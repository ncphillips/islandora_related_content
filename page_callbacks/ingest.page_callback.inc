<?php

function islandora_related_content_ingest_page_callback($object, $cmodel){
  // Replace : with _ in CModel ID.
  $cmid = str_replace(':', '_', $cmodel->id);


  // Implement these hooks if you want to set the collection object.
  $hooks = array(
  "islandora_related_content_ingest",
  "islandora_related_{$cmid}_content_ingest",
  );

  $data = array(
  'collection' => null,
  );
  drupal_alter($hooks, $data);


  // Config data for Ingestion.
  $config_data = array(
  /*
   * id
   * namespace
   * label
   * collections
   * models
   * parent
   */
  );

  $collection = islandora_object_load($data['collection']);

  if($collection){
    // Make sure $collection is a collection, and has a COLLECTION_POLICY.
    $is_collection_object = in_array('islandora:collectionCModel', $collection->models);
    $has_policy = isset($collection['COLLECTION_POLICY']);

    if (!$is_collection_object || !$has_policy) {
      return drupal_not_found();
    }
    $new_object = $collection->repository->constructObject();
    $config_data['collection'] = $collection;
    $config_data['parent'] = $collection->id;
  }
  else {
    $new_object = $object->repository->constructObject();
    $config_data['parent'] = $object;
  }

  $new_object->label = 'New Object';

  $models = array_merge(                    // Models to choose from are
    array($cmodel->id),                     // the CModel passed in and
    islandora_cmodel_subtypes($cmodel->id)  // all of it's Subtypes.
  );

  $config_data['models'] = $models;
  $config_data['object'] = $new_object;

  $new_object->relationships->add(ISLANDORA_RELS_EXT_URI, 'temp', $object);

  module_load_include('inc', 'islandora', 'includes/ingest.form');
  return drupal_get_form('islandora_ingest_form', $config_data);
}