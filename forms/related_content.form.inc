<?php
/**
 * @file
 */

/**
 * @param        $form
 * @param        $form_state
 * @param string $object_id
 * @param string $cmodel_id
 * @param array  $relationships
 *
 * @return mixed
 *
 *
 * # Form State
 * @code
 *  array(
 *    'islandora' = array(
 *      'object_id' = '',
 *      'cmodel_id' = '',
 *      'relationships' = array(''),
 *      'related_objects_ids' = array(''),
 *      'num_found' = 10,
 *      'page' = 0,
 *      'num_pages' = 1,
 *      'solr_filters' = array(''),
 *      'refresh_query' = TRUE,
 *    ),
 *  );
 * @endcode
 */
function islandora_related_content_form($form, &$form_state, $object_id='', $cmodel_id='', $relationships=array()){
  // Some Strings
  $cmodel_id_fns = str_replace(':','_', $cmodel_id);
  $form_div_id = "islandora_related_{$cmodel_id_fns}_form";
  $ajax_callback = 'islandora_related_content_ajax';

  // Formstate
  !empty($form_state['islandora']['object_id']) ? : ($form_state['islandora']['object_id'] = $object_id);
  !empty($form_state['islandora']['cmodel_id']) ? : ($form_state['islandora']['cmodel_id'] = $cmodel_id);
  !empty($form_state['islandora']['relationships']) ? : ($form_state['islandora']['relationships'] = $relationships);
  !empty($form_state['islandora']['form_div_id']) ? : ($form_state['islandora']['form_div_id'] = $form_div_id);
  !empty($form_state['islandora']['ajax_callback']) ? : ($form_state['islandora']['ajax_callback'] = $ajax_callback);

  $cmodel = islandora_cmodel_load($cmodel_id);

  // Wrape the entire form.
  $form['#prefix'] = "<div id='$form_div_id'>";
  $form['#suffix'] = '</div>';

  // Header
  $form['header'] = array(
    '#markup' => $cmodel->label,
    '#prefix' => '<h1>',
    '#suffix' => '</h1>',
  );

  module_load_include('inc', 'islandora_related_content', 'includes/RelatedContentUIFactory');
//  solr_filter_select_field_text_value_element($form, $form_state);

  $solr_factory = new RelatedContentUIFactory($form, $form_state);


  $solr_factory->ingest_element($form, $form_state);
  $solr_factory->add_element($form, $form_state);

  $solr_factory->createContentTable();
//  content_table_select($form, $form_state);

  return $form;
}


function islandora_related_content_form_submit(array $form, array &$form_state) {
  echo "Hello";
  $selected = $form_state['input']['selected'];

  switch($selected){
    case ('remove'):
      $parent_id = $form_state['islandora']['object_id'];
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
                $a = $object->relationships->remove($ns, $rel, $parent_id);
                echo '';
              }
            }
          }
        }
      }
      break;

  }
}


function islandora_related_content_ajax($form, &$form_state) {
  return $form;
}

function page_navigation_element(&$form_state){

}

