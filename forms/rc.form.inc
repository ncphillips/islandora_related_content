<?php

define('RELATIONSHIP_TOWARDS', 'LEFT');
define('RELATIONSHIP_AWAY', 'RIGHT');

function rc_form($form, &$form_state, $object, $cmodel, $relationships, $direction='LEFT'){
  if(!is_object($object)) $object = islandora_object_load($object);
  if(!is_object($cmodel)) $cmodel = islandora_cmodel_load($cmodel);

  $cmodel_safe = str_replace(':','_', $cmodel);
  $form_div_id = "islandora_related_{$cmodel_safe}_content_{$direction}_form";
  $ajax_callback = 'islandora_related_content_ajax';

  // Formstate
  if(empty($form_state['islandora']['object'])) $form_state['islandora']['object'] = $object;
  if(empty($form_state['islandora']['cmodel'])) $form_state['islandora']['cmodel'] = $cmodel;
  if(empty($form_state['islandora']['cmodel_safe'])) $form_state['islandora']['cmodel_safe'] = $cmodel_safe;

  if(empty($form_state['islandora']['relationships'])) $form_state['islandora']['relationships'] = $relationships;
  if(empty($form_state['islandora']['direction'])) $form_state['islandora']['direction'] = $direction;

  if(empty($form_state['islandora']['form_div_id'])) $form_state['islandora']['form_div_id'] = $form_div_id;
  if(empty($form_state['islandora']['current_form'])) $form_state['islandora']['current_form'] = 'related_content';
  if(empty($form_state['islandora']['ajax_callback'])) $form_state['islandora']['ajax_callback'] = $ajax_callback;

  // Wrape the entire form.
  $form['#prefix'] = "<div id='{$form_div_id}'>";
  $form['#suffix'] = '</div>';

  // Header
  $form['header'] = array(
    '#markup' => $cmodel->label,
    '#prefix' => '<h1>',
    '#suffix' => '</h1>',
  );

  // Direction Left is exactly the same as old function.
  // Direction Right
  //  - Ingest and Ad





}
