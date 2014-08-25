<?php
/**
 * @file
 */

/**
 * @param $form
 * @param $form_state
 * @param $object
 * @param $cmodel
 * @param $relationships
 * @param string $direction
 * @return mixed
 *
 * @see hook_form_islandora_related_content_form_alter()
 */
function islandora_related_content_form($form, &$form_state, $object, $cmodel, $relationships, $direction='LEFT'){
  if(!is_object($object)) $object = islandora_object_load($object);
  if(!is_object($cmodel)) $cmodel = islandora_cmodel_load($cmodel);

  $cmodel_safe = str_replace(':','_', $cmodel->id);
  $form_div_id = "islandora_related_{$cmodel_safe}_form";
  $ajax_callback = 'islandora_related_content_ajax';

  // Formstate
  !empty($form_state['islandora']['object']) ? : ($form_state['islandora']['object'] = $object);
  !empty($form_state['islandora']['cmodel']) ? : ($form_state['islandora']['cmodel'] = $cmodel);
  !empty($form_state['islandora']['cmodel_safe']) ? : ($form_state['islandora']['cmodel_safe'] = $cmodel_safe);

  !empty($form_state['islandora']['relationships']) ? : ($form_state['islandora']['relationships'] = $relationships);
  !empty($form_state['islandora']['form_div_id']) ? : ($form_state['islandora']['form_div_id'] = $form_div_id);

  !empty($form_state['islandora']['ajax_callback']) ? : ($form_state['islandora']['ajax_callback'] = $ajax_callback);

  // Wrape the entire form.
  $form['#prefix'] = "<div id='{$form_div_id}'>";
  $form['#suffix'] = '</div>';

  // Header
  $form['header'] = array(
    '#markup' => $cmodel->label,
    '#prefix' => '<h1>',
    '#suffix' => '</h1>',
  );


  module_load_include('inc', 'islandora_related_content', 'includes/RelatedContentUIFactory');
  $related_content_factory = new RelatedContentUIFactory($form, $form_state);

  $related_content_factory->switch_form_link();

  $form['solr_filters'] = array(
    '#prefix' => "<div id='{$form_state['islandora']['form_div_id']}'>",
    '#suffix' => '</div>'
  );


  if ($form_state['islandora']['current_form'] == 'View'){
    $related_content_factory->createContentTable();
  }
  elseif($form_state['islandora']['current_form'] == 'Add'){
    $related_content_factory->fedora_relationship_select();
//    $form['submit'] = array(
//      '#type' => 'submit',
//      '#value' => t('Add Relationship'),
//    );
    $related_content_factory->createContentTable(TRUE, TRUE);
    $form['content_table_select']['actions']['selected']['#options']['add'] = 'Add Relationship';
    $form['content_table_select']['actions']['selected']['#disabled'] = TRUE;
    $form['content_table_select']['actions']['selected']['#default_value'] = 'add';
    $form['content_table_select']['actions']['selected']['#prefix'] = '<div style="display:none;">';
    $form['content_table_select']['actions']['selected']['#suffix'] = '</div>';
    $form['content_table_select']['actions']['submit']['#value'] = 'Add Relationship';
  }
//    $form = array();
//    $form = drupal_build_form('islandora_ingest_form', $form_state);
//    $related_content_factory->switch_form_link();



  return $form;
}

function islandora_related_content_form_submit($form, $form_state){
  $selected = $form_state['values']['selected'];

  switch($selected){
    case ('remove'):
      $parent = $form_state['islandora']['object'];
      foreach($form_state['input']['table'] as $id => $remove_object){
        if($remove_object){
          $object = islandora_object_load($id);
          foreach($form_state['islandora']['relationships'] as $ns_alias => $rels){

            $ns = null;

            switch($ns_alias){
              case 'fedora':
                $ns = FEDORA_RELS_EXT_URI;
                break;
              case 'fedora-model':
                $ns = FEDORA_MODEL_URI;
                break;
              case 'islandora':
                $ns = ISLANDORA_RELS_EXT_URI;
                break;
              case 'islandora-model':
                $ns = ISLANDORA_MODEL_URI;
                break;
            }

            if ($ns){
              foreach($rels as $rel){
                $a = $object->relationships->remove($ns, $rel, $parent);
              }
            }
          }
        }
      }
      break;
    case ('add'):
      $ns = $form_state['values']['namespace'];
      $pred = $form_state['values']['predicate'];

      $obj = $form_state['islandora']['object'];

      foreach($form_state['values']['table'] as $sub_id => $add_rel){
        if ($add_rel){
          $sub = islandora_object_load($sub_id);
          if ($sub){
            $sub->relationships->add($ns, $pred, $obj);
          }
        }
      }
      break;

  }
}


/**
 * Implements hook_form_islandora_related_content_form_alter()
 *
 * Dynamically calls three hooks for altering a related content form:
 *
 *  1. form_islandora_OBJECT_CMODEL_related_objects_form
 *  2. form_islandora_related_RELATED_CMODEL_objects_form
 *  3. form_islandora_OBJECT_CMODEL_related_RELATED_CMODEL_objects_formo
 *
 * Each of these hooks are inherited. So if A is a supertype of B then
 * 'hook_form_islandora_A_related_objects_form_alter()' will also affect
 * the form for B.
 */
function islandora_related_content_form_islandora_related_content_form_alter(&$form, &$formstate){
  $hooks = array();

  // Object
  $object = islandora_object_load($formstate['islandora']['object']);
  if(array_intersect(array('fedora-system:ContentModel-3.0'), $object->models))
  {
    unset($form['Add']);
    unset($form['View']);
  }



  // Related CModel
  $related_cmodel_id = $formstate['islandora']['cmodel'];

  // {Related CModel} UNION {Related CModel Supertypes}
  $related_cmodel_types = islandora_cmodel_types($related_cmodel_id);

  $rcm_count = 0;
  foreach($related_cmodel_types as $rcm){
    // Replace : with _ for hook name generation.
    $rcm_str_replaced  = str_replace(array(':', '.', '-'), '_', $rcm);

    // Replace it in the array of Related CModel Types
    $related_cmodel_types[$rcm_count] = $rcm_str_replaced;

    // Hook function string.
    $hook = "form_islandora_related_{$rcm_str_replaced}_content_form";
    drupal_alter($hook, $form, $formstate);
    $rcm_count++;
  }

  $object_models = islandora_object_models($object);

  // Iterate through the Object's CModels.
  foreach($object_models as $ocm){
    // Replate : with _ for hook name generation.
    $object_cmodel = str_replace(array(':', '.', '-'), '_', $ocm);

    drupal_alter("form_islandora_{$object_cmodel}_related_content_form", $form, $formstate);

    foreach($related_cmodel_types as $rcm){
      $hook = "form_islandora_{$object_cmodel}_related_{$rcm}_content_form";
      drupal_alter($hook, $form, $formstate);
    }
  }
}