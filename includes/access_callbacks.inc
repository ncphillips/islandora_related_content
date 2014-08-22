<?php
/**
 * @file
 */

/**
 * @param $object
 *
 * @return bool
 */
function islandora_related_content_page_access($object){
  $object_is_cmodel = $object->relationships->get(FEDORA_MODEL_URI, 'hasModel', 'fedora-system:ContentModel-3.0');
  $object_is_collection = $object->relationships->get(FEDORA_MODEL_URI, 'hasModel', 'islandora:collectionCModel');

  /** @todo Yeah, this needs to be fixed.... */
  $object_parent_model = islandora_object_parent_model($object);
  $relationships = islandora_cmodel_relationships($object_parent_model);


  if ($object_is_cmodel or $object_is_collection or empty($relationships)){
    return false;
  }
  else {
    return islandora_object_access(ISLANDORA_VIEW_OBJECTS, $object);
  }
}
