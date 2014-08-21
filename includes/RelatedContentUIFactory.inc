<?php



class RelatedContentUIFactory{

  protected $form;
  protected $form_state;

  public function __construct(&$form, &$form_state){
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
  public function createContentTable($has_actions=TRUE, $unrelated=FALSE, $other_filters=''){
    $this->content_table_select_ajax_response();
    $this->get_content($other_filters, $unrelated);

    $cmodel_safe = $this->form_state['islandora']['cmodel_safe'];
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

    if ($this->form_state['islandora']['page'] == 0){
      $element['previous_page']['#disabled'] = TRUE;
    }

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

    if ($this->form_state['islandora']['num_pages'] == $this->form_state['islandora']['page'] + 1){
      $element['next_page']['#disabled'] = TRUE;
    }

    $element['last_page'] = array(
      '#markup' => $this->form_state['islandora']['num_pages'],
    );

//echo '<pre>';
//var_dump($this->form_state['islandora']['related_objects_ids']);
//echo '</pre>';
    // Table Options/Rows
    $options = array();
    foreach ($this->form_state['islandora']['related_objects_ids'] as $related_solr_object) {
      $id = (String)$related_solr_object;
      $object = islandora_object_load($id);
      $options[$object->id] = array(
        'id' => $object->id,
        'label' => $object->label,
        'cmodel' => islandora_object_parent_model($object->id),
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
      '#prefix' => "<div id='solr_filter_$cmodel_safe'>",
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

  private function get_content($other_filters='', $unrelated=false){
    if(empty($this->form_state['islandora']['refresh_query']) or $this->form_state['islandora']['refresh_query']){
        $iqs = new IslandoraSolrQueryset();

        /** Object */
        if(!empty($this->form_state['islandora']['object'])){
          $object = $this->form_state['islandora']['object'];

          /** Relationships
           * Only matter if there's an Object ID. The objects we find must have one
           * of the given relationships towards the Object ID.
           */
          if(!empty($this->form_state['islandora']['relationships'])){
            $relationships = $this->form_state['islandora']['relationships'];
            if($unrelated){
              foreach($relationships as $ns => $rels){
                foreach($rels as $rel){
                  $iqs->fields["RELS_EXT_{$rel}_uri_ms"]['#not'][] = "info:fedora/{$object}";
                }
              }
            }
            else{
              foreach($relationships as $ns => $rels){
                foreach($rels as $rel){
                  $iqs->fields["RELS_EXT_{$rel}_uri_ms"][] = "info:fedora/{$object}";
                }
              }
            }
          }
        }

      /**
       * Content Model
       */
      if (!empty($this->form_state['islandora']['cmodel'])){
        $cmodel = $this->form_state['islandora']['cmodel'];
        $iqs->fields['RELS_EXT_hasModel_uri_ms'][] = 'info:fedora/'. $cmodel;
      }

        /** Current Page */
        if (!empty($this->form_state['islandora']['page'])) {
          $iqs->page = $this->form_state['islandora']['page'];
        }
        else {
          $this->form_state['islandora']['page'] = 0;
        }

        $this->form_state['islandora']['related_objects_ids'] = $iqs->objects;
        $this->form_state['islandora']['num_found'] = $iqs->num_found;
        $this->form_state['islandora']['num_pages'] = $iqs->num_pages;
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

  public function switch_form_link(){
    $add_label = 'Add';
    $view_label = 'View';
    $ingest_label = 'Ingest';

    $all = array ($view_label, $add_label,);

    if(!isset($this->form_state['islandora']['current_form'])){
      $current = $view_label;
    }
    else{
      $current = $this->form_state['islandora']['current_form'];
    }

    if (!empty($this->form_state['triggering_element']['#value'])){
      $this->form_state['islandora']['page'] = 0;
      $v = $this->form_state['triggering_element']['#value'];

      switch ($v) {
        case ($add_label):
          $current = $add_label;
          break;
        case($view_label):
          $current = $view_label;
          break;
        case($ingest_label):
        $current = $ingest_label;
          break;
      }
    }

    $this->form_state['islandora']['current_form'] = $current;

    $object = $this->form_state['islandora']['object'];
    $can_add    = islandora_object_access(ISLANDORA_METADATA_EDIT,     $object)
    and islandora_object_access(ISLANDORA_MANAGE_PROPERTIES, $object);

    foreach ($all as $label){
      $this->form[$label] = array(
        '#type' => 'button',
        '#value' => $label,
        '#ajax' => array(
          'callback' => 'ajax_return_related_content_form',
          'wrapper' => $this->form_state['islandora']['form_div_id'],
          'effect' => 'fade',
        ),
        '#disabled' => (($label != $current) ? false : true),
        '#access' => ($label == $add_label) ? $can_add : true,
      );
    }

    $this->form[$ingest_label] = array(
      '#type' => 'markup',
      '#markup' => l($ingest_label, "islandora/object/{$this->form_state['islandora']['object']}/ingest/{$this->form_state['islandora']['cmodel']}"),
      '#prefix' => '<span class="button">',
      '#suffix' => '</span>',
    );
  }

  /**
   * Unfortunately, I'm going to make this function only work with the fedora-system
   * namespace for now.
   */
  public function fedora_relationship_select(){
    $form = &$this->form;
    $form_state = &$this->form_state;
    $relationships = $this->form_state['islandora']['relationships'];

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
    foreach($relationships as $alias => $rel){
      switch($alias){
        case 'fedora':
        case 'fedora-system':
          $namespaces[FEDORA_RELS_EXT_URI] = $alias;
          break;
        case 'fedora-model':
          $namespaces[FEDORA_MODEL_URI] = $alias;
          break;
        case 'islandora':
          $namespaces[ISLANDORA_RELS_EXT_URI] = $alias;
          break;
        case 'islandora-model':
          $namespaces[ISLANDORA_MODEL_URI] = $alias;
          break;
        default:
            break;
      }
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
      $ns = array_diff(array_keys($namespaces), array(''))[1];
      $form_state['islandora']['namespace'] = array($ns, $namespaces[$ns]);
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
      foreach($relationships as $alias => $rels){
        foreach($rels as $rel){
          if($alias == $form_state['islandora']['namespace'][1]){
            $p = $rel;
            $predicates[$p] = $p;
          }
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

function ajax_return_related_content_form($form, &$form_state){
  return $form;
}
