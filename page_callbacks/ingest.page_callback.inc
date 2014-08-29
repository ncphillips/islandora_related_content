<?php
/**
 * @file
 */

/**
 * @param $object
 * @param $cmodel
 * @return array|mixed|void@
 *
 * @see hook_islandora_related_content_ingest_alter()
 * @see hook_islandora_related_RELATED_CMODEL_content_ingest_alter()
 */
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
   * @see islandora_ingest_form()
   * id
   * namespace
   * label
   * collections
   * models
   * parent
   */
  );

  $new_object = $object->repository->constructObject();
  $config_data['objects'] = array($new_object);

  $collection = islandora_object_load($data['collection']);

  if($collection){
    // Make sure $collection is a collection, and has a COLLECTION_POLICY.
    $is_collection_object = in_array('islandora:collectionCModel', $collection->models);
    $has_policy = isset($collection['COLLECTION_POLICY']);

    if (!$is_collection_object || !$has_policy) {
      return drupal_not_found();
    }
    $config_data['collections'] = array($collection);
    $new_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $collection);

    $policy = new CollectionPolicy($collection['COLLECTION_POLICY']->content);
    $collection_models = $policy->getContentModels();

    $cmodel_family = islandora_cmodel_types($cmodel);

    $a = array_intersect($cmodel_family, array_keys($collection_models));

    $collection_model = $collection_models[$a[0]];
    $namespace = islandora_get_namespace($collection_model['namespace']);
  }
  else{
    preg_match('/^(.*):.*/', $object->id, $matches);
    $namespace = $matches[1];
  }

  $new_object->id = $new_object->repository->getNextIdentifier($namespace, variable_get('islandora_basic_collection_generate_uuid', FALSE));

  $config_data['models'] = array($cmodel->id);
  $config_data['parent'] = $object;

  module_load_include('inc', 'islandora', 'includes/ingest.form');
  $form = drupal_get_form('islandora_ingest_form', $config_data);

  return $form;
}