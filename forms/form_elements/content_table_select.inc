<?php

function content_table_select_new(&$form, &$form_state, $has_actions=TRUE){
  $object = NULL;
  $relationships = array();
  $solr_filters = array();

  /** Object ID
   * If it's there, the objects we find must have a relationship to it,
   * otherwise we don't care about the objects relationships we find towards
   * anything.
   */
  if(!empty($form_state['islandora']['object'])){
    $object = $form_state['islandora']['object'];

    /** Relationships
     * Only matter if there's an Object ID. The objects we find must have one
     * of the given relationships towards the Object ID.
     */
    if(!empty($form_state['islandora']['relationships'])){
      $relationships = $form_state['islandora']['relationships'];
    }
  }

  /**


  /** Solr Fitlers
   * This key-value array has the Solr query filter as the key, and
   * a label for that filter as the value. If a label is provided then a
   * UI element will be displayed which allows it to be removed.
   * If the label is an empty string then this UI element will not be shown.
   *
   * @code
   * array(
   *  'field:value' => 'Label',
   *  '(field:value or field:value)' => 'Diff Label',
   *  'field:value' => '',
   * );
   * @endcode
   */


  /**
   * The Table
   */
}


/**
 * @param $form
 * @param $form_state
 * @param bool $has_actions
 *
 *
 */
function content_table_select(&$form, &$form_state, $has_actions=TRUE){

  /**
   * If no $object_id is provided then this table will load all objects
   * of the given Content Model.
   */
  if(!empty($form_state['islandora']['object_id'])){
    $object = islandora_object_load($form_state['islandora']['object_id']);

    if (!empty($form_state['islandora']['relationships']) ){
      $relationships = $form_state['islandora']['relationships'];
    }
    else{
      $relationships = array();
    }
  }
  else{
    $object = '';
    $relationships = array();
  }

  !empty($form_state['islandora']['cmodel_id']) ? : ($form_state['islandora']['cmodel_id'] = '');
  !empty($form_state['islandora']['form_div_id']) ? : ($form_state['islandora']['form_div_id'] = '');
  !empty($form_state['islandora']['refresh_query']) ? : ($form_state['islandora']['refresh_query'] = TRUE);

  !empty($form_state['islandora']['solr_filters']) ? : ($form_state['islandora']['solr_filters'] = array());
  !empty($form_state['islandora']['page']) ? : ($form_state['islandora']['page'] = 0);

  if (!empty($form_state['islandora']['ajax_callback'])){
    $ajax_callback = $form_state['islandora']['ajax_callback'];
  }
  else {
    $ajax_callback = 'content_table_select_ajax_return';
  }



  $cmodel_id = $form_state['islandora']['cmodel_id'];

  $cmodel_id_fns = str_replace(':', '_', $form_state['islandora']['cmodel_id']);
  $form_div_id = $form_state['islandora']['form_div_id'];

  content_table_select_ajax_response($form_state);

  $element['current_filters'] = array(
    '#prefix' => "<div id='current_solr_filters_{$form_div_id}'>",
    '#suffix' => '</div>',
  );

  foreach($form_state['islandora']['solr_filters'] as $filter){
    $element['current_filters'][$filter] = array(
      '#type' => 'button',
      '#value' => $filter,
      '#prefix' => '<div class="solr_filter">',
      '#suffix' => '</div>',
      '#ajax' => array(
        'callback' => $ajax_callback,
        'wrapper' => $form_div_id,
      ),
    );
  }


  // Execut Solr Query
  if($form_state['islandora']['refresh_query']){
    // Add Filters.
    if($form_state['islandora']['solr_filters']){
      $other_queries = '('.implode(' AND ', $form_state['islandora']['solr_filters']).')';
    }
    else {
      $other_queries = '';
    }

    if($object) $query_args['object'] = $object->id;
    if($cmodel_id) $query_args['cmodels'] = array($cmodel_id);
    if($relationships) $query_args['relationships'] = $relationships;
    if($other_queries) $query_args['other'] = $other_queries;
//    $query_args['page'] = $form_state['islandora']['page'];

    $related_objects_ids = array();

    // Execute Query
    for($i=0; $i<=$form_state['islandora']['page']; $i++){
      $query_args['page'] = $i;
      $related_objects_query_results = islandora_object_related_islandora_objects($query_args);
      $related_objects_ids = array_merge($related_objects_ids, $related_objects_query_results['ids']);
    }


    // Add values to the $form_state
    $form_state['islandora']['related_objects_ids'] = $related_objects_ids;
    $form_state['islandora']['num_found'] = $related_objects_query_results['num_found'];
    $form_state['islandora']['num_pages'] = $related_objects_query_results['num_pages'];
  }
  else{
    // Otherwise fetch them.
    $related_objects_ids = $form_state['islandora']['related_objects_ids'];
  }

  if ($has_actions){
    // Actions
    $element['actions']['selected'] = array(
      '#type' => 'select',
      '#title' => 'Actions',
      '#options' => array(
        'remove' => t('Remove'),
      ),
      //    '#description' => t('Actions'),
    );

    // Submit button for form actions.
    $element['actions']['submit'] =  array(
      '#type' => 'submit',
      '#value' => 'Submit',
      '#submit' => array('content_table_select_actions_submit'),
    );
  }

  // Related Objects Table
  $element['previous_page'] = array(
    '#type' => 'button',
    '#value' => 'previous',
    '#ajax' => array(
      'callback' => 'content_table_select_ajax_return',
      'wrapper' => $form_div_id,
      'method' => 'replace',
    ),
  );

  $element['page_num'] = array(
    '#markup' => $form_state['islandora']['page'] + 1,
  );

  $element['next_page'] = array(
    '#type' => 'button',
    '#value' => 'next',
    '#ajax' => array(
      'callback' => 'content_table_select_ajax_return',
      'wrapper' => $form_div_id,
      'method' => 'replace',
    ),
  );

  $element['last_page'] = array(
    '#markup' => $form_state['islandora']['num_pages'],
  );

  $header = array(
    'id' => 'ID',
    'label' => 'Label',
    'cmodel' => 'Type',
  );

  // Table Options/Rows
  $options = array();
  foreach ($related_objects_ids as $related_object_id) {
    $related_object = islandora_object_load($related_object_id);
    $options[$related_object_id] = array(
      'id' => $related_object_id,
      'label' => $related_object->label,
      'cmodel' => islandora_object_parent_model($related_object_id),
    );
  }

  // Table
  $element['table'] = array(
    '#type' => 'tableselect',
    '#header' => array(
      'id' => 'ID',
      'label' => 'Label',
      'cmodel' => 'Type',
    ),
    '#options' => $options,
    '#empty' => t("Null."),
    '#prefix' => "<div id='solr_filter_$cmodel_id_fns'>",
    '#suffix' => '</div>',
  );

  $form['content_table_select'] = $element;
}

function content_table_select_actions_submit(&$form, &$form_state){
  $object_id = $form_state['islandora']['object_id'];
  $object = islandora_object_load($object_id);
  $relationships = $form_state['islandora']['relationships'];

  // Checked values in the table.
  $table_values = $form_state['values']['table'];

  // Action selected in teh drop down.
  $selected_option = $form_state['values']['selected'];
  switch ($selected_option) {
    case "remove":
      foreach ($table_values as $lines => $selected_pid) {
        if ($selected_pid) {
          $related_object = islandora_object_load($selected_pid);

          foreach($relationships as $pred){
            $related_object->relationships->remove(NULL, $pred, $object_id);
          }
        }
      }
      break;
  }
}


function content_table_select_ajax_response(&$form_state){
  if(empty($form_state['triggering_element'])) return;

  // AJAX
  $i = &$form_state['islandora'];
  $trigger = &$form_state['triggering_element'];

  if(!empty($trigger['#prefix']) and $trigger['#prefix'] == '<div class="solr_filter">'){
    $filter = $trigger['#value'];
    unset($i['solr_filters'][$filter]);
  }

  // Other buttons
  if(!empty($trigger['#value'])){
    // Next Page
    if($trigger['#value'] == 'next'){
      if($i['page']+1 < $i['num_pages']){
        $i['page']++;
      }
      else {
        $i['refresh_query'] = FALSE;
      }
    }

    // Previous Page
    elseif($trigger['#value'] == 'previous'){
      if($i['page'] > 0) {
        $i['page']--;
      }
      else {
        $i['refresh_query'] = FALSE;
      }
    }
  }
}





function content_table_select_ajax_return(&$form, &$form_state){
  return $form;
}
