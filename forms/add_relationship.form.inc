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

  module_load_include('inc', 'islandora_related_content', 'forms/form_elements/islandora_related_content_form_elements');
  fedora_relationship_select($form, $form_state, $relationships);

  $cmodels_and_labels = array(''=>'All');
  foreach(islandora_cmodel_subtypes($cmodel) as $cm){
    $cm_obj = islandora_cmodel_load($cm);
    $cmodels_and_labels[$cm] = $cm_obj->label;
  }

  solr_filter_field_select($form, $form_state,
    array('RELS_EXT_hasModel_uri_mt', 'Specimen Type'),
    $cmodels_and_labels
  );
  solr_filter_select_field_text_value_element($form, $form_state);

  content_table_select($form, $form_state, FALSE);

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

