<?php

function islandora_related_content_page_callback($object, $related_cmid_restriction=null){
  $page = array();

  // Replace : with _ in CModel ID.
  $object_parent_model = islandora_object_parent_model($object->id);
  $objects_models = islandora_object_models($object->id);

  // Page State
  $page_state['object'] = $object;
  $page_state['relationships'] = islandora_cmodel_relationships($object_parent_model);

  // Hooks to call.
  $hooks = array('islandora_related_objects_page');

  // Related Content Page Alters are inherited.
  // It would be nice to make sure order is preserved.
  foreach($objects_models as $m){
    $o = str_replace(':', "_", $m);
    $hooks[] = "islandora_{$o}_related_objects_page";// Alter the related objects page for $cmid.

  }

  drupal_alter($hooks, $page, $page_state);

  // One Related Content Form per CModel.
  $related_content_form_args = array();
  foreach($page_state['relationships'] as $relationship){
    if(array_intersect(array($relationship['object']['value']), $objects_models)){
      $related_cmodel_id = $relationship['subject']['value'];

      if($related_cmid_restriction == null or $related_cmid_restriction == $related_cmodel_id){
        $alias = $relationship['predicate']['alias'];
        if ($alias == 'fedora-system')
          $alias = 'fedora';

        $pred = $relationship['predicate']['value'];

        $related_content_form_args[$related_cmodel_id][$alias][] = $pred;
      }
    }
  }


  module_load_include('inc', 'islandora_related_content', 'forms/rc.form');
  // Create the Related Content Forms
  foreach($related_content_form_args as $related_cmodel_id => $rels){
    $page['content forms'][] = drupal_get_form(
      'islandora_related_content_form', // Form ID
      $object->id,                      // Object ID
      $related_cmodel_id,               // Related CModel ID
      $rels                             // Array of valid relationships
    );
  }

  return $page;
}
