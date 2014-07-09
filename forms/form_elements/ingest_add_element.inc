<?php
/**
 * Created by PhpStorm.
 * User: nolan
 * Date: 2014-07-07
 * Time: 8:51 AM
 */

function ingest_element(&$form, &$form_state){
  !empty($form_state['islandora']['object_id']) ? : ($form_state['islandora']['object_id'] = '');
  !empty($form_state['islandora']['cmodel_id']) ? : ($form_state['islandora']['cmodel_id'] = '');

  $object = islandora_object_load($form_state['islandora']['object_id']);
  $cmodel = islandora_cmodel_load($form_state['islandora']['cmodel_id']);

  $can_ingest = islandora_object_access(ISLANDORA_INGEST, $object);

  $form['ingest_link'] = array(
    '#type' => 'markup',
    '#markup' =>  l("Ingest {$cmodel->label}",
      "islandora/object/{$object->id}/related_content/ingest/{$cmodel->id}"),
    '#prefix' => '<div>',
    '#suffix' => '</div>',
    '#access' => $can_ingest
  );
}

function add_element(&$form, &$form_state){
  !empty($form_state['islandora']['object_id']) ? : ($form_state['islandora']['object_id'] = '');
  !empty($form_state['islandora']['cmodel_id']) ? : ($form_state['islandora']['cmodel_id'] = '');

  $object = islandora_object_load($form_state['islandora']['object_id']);
  $cmodel = islandora_cmodel_load($form_state['islandora']['cmodel_id']);

  $can_add    = islandora_object_access(ISLANDORA_METADATA_EDIT,     $object)
  and islandora_object_access(ISLANDORA_MANAGE_PROPERTIES, $object);

  $form['add_link'] = array(
    '#type' => 'markup',
    '#markup' =>  l("Add Existing {$cmodel->label}",
      "islandora/object/{$object->id}/related_content/add/{$cmodel->id}"),
    '#prefix' => '<div>',
    '#suffix' => '</div>',
    '#access' => $can_add
  );

}