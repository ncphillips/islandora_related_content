<?php



class RelatedContentUIFactory{

  protected $form;
  protected $form_state;

  public function __construct(&$form, &$form_state){
    $this->num_widgets = 0;
    $this->form = &$form;
    $this->form_state = &$form_state;
  }

  /**
   * Create Select-Text Widget
   *
   * @param $fields
   * @code
   *    array(
   *      'Field01' => 'Field Label 01',
   *      'Field02' => 'Field Label 02',
   *    );
   * @endcode
   *
   * A Solr Filter Widget. The a list of fields to choose from is provided as a
   * dropdown menu, and the user inputs the filter in the a text field.
   */
  public function createSelectTextWidget($fields){
    $i = &$this->form_state['islandora'];
    $values = &$this->form_state['values'];

    $id = $i['form_div_id'] . '_solr_select_text_widget';

    // Create Widget
    $element = array(
      '#prefix' => "<div id='$id'>",
      '#suffix' => '</div>',
    );

    $element["{$id}_select"] = array(
      '#type' => 'select',
      '#title' => 'Field',
      '#options' => $fields,
    );

    $element["{$id}_textfield"] = array(
      '#type' => 'textfield',
      '#default_value' => '*',

    );

    $element["{$id}_apply"] = array(
      '#type' => 'button',
      '#value' => 'Apply Filter',
      '#ajax' => array(
        'callback' => 'ajax_return_select_text_and_content_table_select',
      ),
    );

    // Handle AJAX Events
    if(!empty($this->form_state['triggering_element']['#value']) ){

      // Applying a filter.
      if($this->form_state['triggering_element']['#parents'][0] == "{$id}_apply"){
        $v = str_replace(':', '\:', $this->form_state['values']["{$id}_textfield"]);
        $filter =  $this->form_state['values']["{$id}_select"] . ':' . $v;
        $i['solr_filters'][$id][$filter] = $fields[$values["{$id}_select"]].':'.$v;
        $i['page'] = 0;
      }

      // Removing a filter.
      elseif($this->form_state['triggering_element']['#array_parents'][0] == "solr_select_text_widget"){
        $filter = $this->form_state['triggering_element']['#parents'][0];
        unset($i['solr_filters'][$id][$filter]);
      }
    }

    // Show current Filters.
    if (!empty($i['solr_filters'])){
      foreach($i['solr_filters'][$id] as $filter => $label){
        $element['current_filters'][$filter] = array(
          '#type' => 'button',
          '#value' => $label,
          '#ajax' => array(
            'callback' => 'ajax_return_select_text_and_content_table_select',
          ),
        );
      }
    }

    $this->form['solr_select_text_widget'] = $element;
  }

  /**
   * Create Label-Select Widget
   *
   * @param $field
   * @code
   *    array(
   *      'solr_field',
   *      'Label',
   *    );
   * @endcode
   *
   * @param $values
   * @code
   *    array(
   *      'value 01' => 'label 01',
   *      'value 02' => 'label 02',
   *    );
   * @endcode
   */
  public function createLabelSelectWidget($field, $options){
    $i = &$this->form_state['islandora'];
    $values = &$this->form_state['values'];

    $id = $i['form_div_id'] . 'solr_label_select_widget';

    $element["{$id}_label"] = array(
      '#markup' => $field[1]
    );

    if (!in_array('*', array_keys($options))){
      $options['*'] = 'Any';
    }

    $element["{$id}_select"] = array(
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => '*',
      '#ajax' => array(
        'callback' => 'ajax_return_content_table_select',
        'wrapper' => $this->form_state['islandora']['form_div_id'] . '_content_table_select',
      ),
    );

    // Handles AJAX
    if(!empty($this->form_state['triggering_element']['#value']) ){

      // Applying a filter.
      if($this->form_state['triggering_element']['#parents'][0] == "{$id}_select"){
        $v = str_replace(':', '\:', $values["{$id}_select"]);
        $filter =  $field[0] . ':' . $v;
        unset($i['solr_filters'][$id]);
        $i['solr_filters'][$id][$filter] = '';
        $i['page'] = 0;
      }
    }

    $this->form['solr_label_select_widget'] = $element;

  }

  /**
   * Create Text Widget
   */
  public function createTextWidget(){
    /** Incomplete */
    $element = array(
      '#type' => 'textfield',
      '#default_value' => '*',

    );

    $this->form['solr_filter_text'] = $element;
  }

  /** Create Content Table  */
  public function createContentTable($has_actions=TRUE){
    $this->content_table_select_ajax_response();
    $this->get_content();

    $cmodel_id_fns = str_replace(':', '_', $this->form_state['islandora']['cmodel_id']);
    $form_div_id = $this->form_state['islandora']['form_div_id'];

    $id = $form_div_id . '_content_table_select';

    $element['#prefix'] = "<div id='{$id}'>";
    $element['#suffix'] = '</div>';

    // Actions
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

    // Navigation
    $element['previous_page'] = array(
      '#type' => 'button',
      '#value' => 'previous',
      '#ajax' => array(
        'callback' => 'ajax_return_content_table_select',
        'wrapper' => $id,
        'method' => 'replace',
      ),
    );

    $element['page_num'] = array(
      '#markup' => $this->form_state['islandora']['page'] + 1,
    );

    $element['next_page'] = array(
      '#type' => 'button',
      '#value' => 'next',
      '#ajax' => array(
        'callback' => 'ajax_return_content_table_select',
        'wrapper' => $id,
        'method' => 'replace',
      ),
    );

    $element['last_page'] = array(
      '#markup' => $this->form_state['islandora']['num_pages'],
    );

    // Table Options/Rows
    $options = array();
    foreach ($this->form_state['islandora']['related_objects_ids'] as $related_object_id) {
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


    $this->form['content_table_select'] = $element;
  }


  private function content_table_select_ajax_response(){
    if(empty($this->form_state['triggering_element'])) return;

    // AJAX
    $i = &$this->form_state['islandora'];
    $trigger = &$this->form_state['triggering_element'];

    // Navigation
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

  private function get_content(){
    if(empty($this->form_state['islandora']['refresh_query']) or $this->form_state['islandora']['refresh_query']){
      $query_args = array();

      /** Object */
      if(!empty($this->form_state['islandora']['object_id'])){
        $query_args['object'] = $this->form_state['islandora']['object_id'];

        /** Relationships
         * Only matter if there's an Object ID. The objects we find must have one
         * of the given relationships towards the Object ID.
         */
        if(!empty($this->form_state['islandora']['relationships'])){
          $query_args['relationships'] = $this->form_state['islandora']['relationships'];
        }
      }

      /**
       * Content Model
       */
      if (!empty($this->form_state['islandora']['cmodel_id'])){

        $query_args['cmodels'] = array($this->form_state['islandora']['cmodel_id']);
      }

      /** Solr Fitlers
       *
       *
       * @code
       * $this->form_state['islandora']['solr_filters'] = array(
       *    'widget_html_id' => array(
       *      'field:value' => array(
       *        'param1' => 'a',
       *        'param2' => 'b',
       *    ),
       *    'solr_select_text_widget' => array(
       *      'RELS_EXT_fedora_hasModel_uri_mt:islandora\:invertebrate_cmodel' => array(
       *        'label' => 'Specimen Type: Invertebrate',
       *      ),
       *    ),
       * );
       * @endcode
       */
      if(!empty($this->form_state['islandora']['solr_filters'])){
        $filters_array= array();
        // For each Solr Filter Widget
        foreach($this->form_state['islandora']['solr_filters'] as $sf_widgets){
          foreach($sf_widgets as $filter => $params){
            $filters_array[] = $filter;
          }
        }
        if($filters_array) {
          $filters_query = '(' . implode(' AND ', $filters_array) . ')';
          $query_args['other'] = $filters_query;
        }
      }

      /** Current Page */
      if (!empty($this->form_state['islandora']['page'])) {
        $query_args['page'] = $this->form_state['islandora']['page'];
      }
      else{
        $this->form_state['islandora']['page'] = 0;
      }

      /** Execute the Solr Query */
      $related_content = related_islandora_objects($query_args);
      $this->form_state['islandora']['related_objects_ids'] = $related_content['ids'];
      $this->form_state['islandora']['num_found'] = $related_content['num_found'];
      $this->form_state['islandora']['num_pages'] = $related_content['num_pages'];

    }
  }

  public function ingest_element(){
    !empty($this->form_state['islandora']['object_id']) ? : ($form_state['islandora']['object_id'] = '');
    !empty($this->form_state['islandora']['cmodel_id']) ? : ($form_state['islandora']['cmodel_id'] = '');

    $object = islandora_object_load($this->form_state['islandora']['object_id']);
    $cmodel = islandora_cmodel_load($this->form_state['islandora']['cmodel_id']);

    $can_ingest = islandora_object_access(ISLANDORA_INGEST, $object);

    $this->form['ingest_link'] = array(
      '#type' => 'markup',
      '#markup' =>  l("Ingest {$cmodel->label}",
        "islandora/object/{$object->id}/related_content/ingest/{$cmodel->id}"),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#access' => $can_ingest
    );
  }

  public function add_element(){
    !empty($this->form_state['islandora']['object_id']) ? : ($form_state['islandora']['object_id'] = '');
    !empty($this->form_state['islandora']['cmodel_id']) ? : ($form_state['islandora']['cmodel_id'] = '');

    $object = islandora_object_load($this->form_state['islandora']['object_id']);
    $cmodel = islandora_cmodel_load($this->form_state['islandora']['cmodel_id']);

    $can_add    = islandora_object_access(ISLANDORA_METADATA_EDIT,     $object)
    and islandora_object_access(ISLANDORA_MANAGE_PROPERTIES, $object);

    $this->form['add_link'] = array(
      '#type' => 'markup',
      '#markup' =>  l("Add Existing {$cmodel->label}",
        "islandora/object/{$object->id}/related_content/add/{$cmodel->id}"),
      '#prefix' => '<div>',
      '#suffix' => '</div>',
      '#access' => $can_add
    );
  }


  public function fedora_relationship_select(&$form, &$form_state, $relationships){
    // Wrap this element in a div.
    $element['#prefix'] = "<div id='fedora_relationship_select'>";
    $element['#suffix'] = "</div>";

    /**
     * @var  $namespaces
     * An array of key-value pairs where the key is the namespace URI, and the
     * value is the alias for that namespace.
     * @code
     * array(
     *  'info:fedora/fedora-system:def/relations-external#' => 'fedora-system',
     *  'http://www.w3.org/1999/02/22-rdf-syntax-ns#' => 'rdf',
     * ),
     * @endcode
     */
    $namespaces = array(''=> t('< Namespace >'), );
    foreach($relationships as $pred_obj){
      $ns_uri = $pred_obj['predicate']['namespace'];
      $alias = $pred_obj['predicate']['alias'];

      $namespaces[$ns_uri] = $alias;
    }

    // NAMESPACE SELECTOR
    $element['namespace'] = array(
      '#type' => 'select',
      '#options' => $namespaces,
      '#ajax' => array(
        'callback' => 'ajax_return_fedora_relationship_select',
        'wrapper' => 'fedora-relationship-predicate',
      ),
      '#prefix' => '<div id="namespace_select">',
      '#suffix' => '</div>',
    );

    if (sizeof($namespaces) == 2){
      $ns = array_diff(array_keys($namespaces), array(''));
      $form_state['islandora']['namespace'] = $ns[1];
      $element['namespace']['#prefix'] = '<div id="namespace_select" style="display:none;">';
      $element['namespace']['#suffix'] = '</div>';
      $element['namespace']['#default_value'] = $ns;
    }
    elseif(!empty($form_state['values'])){
      $form_state['islandora']['namespace'] = $form_state['values']['namespace'];
    }
    else {
      $form_state['islandora']['namespace'] = '';
    }


    // PREDICATE SELECTOR
    $element['predicate'] = array(
      '#type' => 'select',
      '#prefix' => '<div id="fedora-relationship-predicate">',
      '#suffix' => '</div>',
      '#options' => array(''=> t('< Relationship >')),
      '#disabled' => true,
    );

    if ($form_state['islandora']['namespace']){
      $predicates  = array();
      foreach($relationships as $pred_obj){
        if($pred_obj['predicate']['namespace'] == $form_state['islandora']['namespace']){
          $p = $pred_obj['predicate']['value'];
          $predicates[$p] = $p;
        }
      }

      $element['predicate']['#options'] = $predicates;
      $element['predicate']['#disabled'] = false;
    }

    // ADD TO FORM
    $form['fedora_relationship_select'] = $element;
  }
}

function ajax_return_content_table_select($form, $form_state){
  return $form['content_table_select'];
}

function ajax_return_select_text_and_content_table_select($form, $form_state){

  $sf_id = $form_state['islandora']['form_div_id'] . '_solr_select_text_widget';
  $sf_element = render($form['solr_select_text_widget']);

  $ct_id = $form_state['islandora']['form_div_id'] . '_content_table_select';
  $ct_element = render($form['content_table_select']);

  return array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace('#'.$sf_id, $sf_element),
      ajax_command_replace('#'.$ct_id, $ct_element),
    ),
  );
}

/**
 * @param $form
 * @param $form_state
 * @return mixed
 * Drupal form element.
 */
function ajax_return_fedora_relationship_select($form, &$form_state){
  return $form['fedora_relationship_select']['predicate'];
}
