<?php

function add_relationship_form($form, &$form_state, $object, $cmodel){
  !empty($form_state['islandora']['target_id']) ? : ($form_state['islandora']['target_id'] = $object->id);
  !empty($form_state['islandora']['object_id']) ? : ($form_state['islandora']['object_id'] = '');
  !empty($form_state['islandora']['cmodel_id']) ? : ($form_state['islandora']['cmodel_id'] = $cmodel->id);

  $cmodel_id_fns = str_replace(':','_', $cmodel->id);
  $form_div_id = "islandora_add_{$cmodel_id_fns}_form";

  !empty($form_state['islandora']['form_div_id']) ? : ($form_state['islandora']['form_div_id'] = $form_div_id);

  $form['#prefix'] = "<div id='{$form_div_id}'>";
  $form['#suffix'] = '</div>';

  $object_cm = islandora_object_parent_model($object);
  $relationships = islandora_cmodel_relationships_towards($object_cm);

  foreach($relationships as $index => $pred_obj){
    if ($pred_obj['object']['value'] != $cmodel->id){
      unset($relationships[$index]);
    }
  }

  module_load_include('inc', 'islandora_related_content', 'includes/RelatedContentUIFactory');
  $rc_factory = new RelatedContentUIFactory($form, $form_state);
  $rc_factory->fedora_relationship_select($form, $form_state, $relationships);

  $cmodels_and_labels = array(''=>'All');
  foreach(islandora_cmodel_subtypes($cmodel) as $cm){
    $cm_obj = islandora_cmodel_load($cm);
    $cmodels_and_labels[$cm] = $cm_obj->label;
  }

  $fields_array = array();
  foreach($relationships as $pred_obj){
    $value = $pred_obj['predicate']['value'];
    $alias = ($pred_obj['predicate']['alias'] == 'fedora-system') ? 'fedora' : $pred_obj['predicate']['alias'];
    $fields_array[] = "RELS_EXT_{$value}_uri_mt:" . '"' . $object->id . '"';
    $fields_array[] = "RELS_EXT_{$alias}_{$value}_uri_mt:" . '"' . $object->id . '"';
    $fields_array[] = "RELS_EXT_{$value}_uri_mt:" . '"info:fedora/' . $object->id . '"';
    $fields_array[] = "RELS_EXT_{$alias}_{$value}_uri_mt:" . '"info:fedora/' . $object->id . '"';
  }

//echo '<pre>';
//var_dump($fields_array);
//echo '</pre>';

  if ($fields_array)
    $fields = '( *:* NOT ('.implode(' OR ', $fields_array).'))';

  $rc_factory->createContentTable(FALSE, $fields);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Create Relationship',
    '#submit' => array('add_relationship_form_submit'),
  );

  if (empty($form_state['islandora']['namespace'])){
    $form['submit']['#disabled'] = TRUE;
  }

  return $form;
}

function add_relationship_form_submit($form, &$form_state){
echo 'Submit';
var_dump($form);
  $ns = $form_state['values']['namespace'];
  $pred = $form_state['values']['predicate'];

  $obj = $form_state['islandora']['target_id'];

  foreach($form_state['values']['table'] as $sub_id => $selected){
    if ($selected){
      $sub = islandora_object_load($sub_id);
      if ($sub){
        $sub->relationships->add($ns, $pred, $obj);
      }
    }
  }
}

