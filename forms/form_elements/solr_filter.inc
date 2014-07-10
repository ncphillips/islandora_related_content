<?php


class SolrQueryFilterUIFactory{

  protected $form;
  protected $form_state;

  protected $num_widgets;

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
    $id = 'solr_select_text_widget_' . $this->num_widgets;

    $i = &$this->form_state['islandora'];
    $values = &$this->form_state['values'];

    // Handle AJAX Events
    if(!empty($this->form_state['triggering_element']['#value']) and
      $this->form_state['triggering_element']['#value'] == 'Apply Filter'){
      $v = str_replace(':', '\:', $this->form_state['values']["{$id}_textfield"]);
      $filter =  $this->form_state['values']["{$id}_select"] . ':' . $v;
      $i['solr_filters'][$filter] = $filter;
      $i['page'] = 0;
    }

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
        'callback' => 'solr_filter_element_return',
        'wrapper' => $this->form_state['islandora']['form_div_id'],
      ),
    );

    $this->form[$id] = $element;
    $this->num_widgets++;
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
  public function createLabelSelectWidget($field, $values){
    $id = 'solr_label_select_widget' . $this->num_widgets;

    $element["{$id}_label"] = array(
      '#markup' => $field[1]
    );

    $element['solr_filter_field_select'] = array(
      '#type' => 'select',
      '#options' => $values,
      '#ajax' => array(
        'callback' => 'solr_filter_field_select_ajax_return',
        'wrapper' => $this->form_state['islandora']['form_div_id'],
      ),
    );

    if (!empty($form_state['values'])){
      if($form_state['values']['solr_filter_field_select']){
        $filter = $field[0] . '\:' . $form_state['values']['solr_filter_field_select'];
        $form_state['islandora']['solr_filters'][$filter] = $filter;
      }
    }

    $form['solr_filter_field'] = $element;

  }
}




function solr_filter_text(&$form, &$form_state){
  $element = array();

  $form['solr_filter_text'] = $element;
}

function solr_filter_select_field_text_value_element(&$form, &$form_state){
  !empty($form_state['islandora']['form_div_id']) ? : ($form_state['islandora']['form_div_id'] = '');
  !empty($form_state['islandora']['solr_filters']) ? : ($form_state['islandora']['solr_filters'] = array());
  !empty($form_state['islandora']['ajax_callback']) ? : ($form_state['islandora']['ajax_callback'] = 'solr_filter_element_return');

  $ajax_callback = $form_state['islandora']['ajax_callback'];
  $form_div_id = $form_state['islandora']['form_div_id'];
  $solr_div_id = 'solr_filters_' . $form_div_id;

  solr_filter_element_ajax_response($form_state);

  $element = array(
    '#prefix'=> "<div id='{$solr_div_id}'>",
    '#suffix'=> '</div>',
  );

  $element['solr_field'] = array(
    '#type' => 'select',
    '#title' => t('Field'),
    '#options' => array(
      'PID' => 'PID',
      'RELS_EXT_hasModel_uri_mt' => 'Content Type',
    ),
  );

  $element['solr_value'] = array(
    '#type' => 'textfield',
    '#default_value' => '*',
    '#title' => 'Value',
  );

  $element['apply_filter'] = array(
    '#type' => 'button',
    '#value' => 'Apply Filter',
    '#ajax' => array(
      'callback' => $ajax_callback,
      'wrapper' => $form_div_id,
      'method' => 'replace',
    ),
  );



  $form['solr_filter'] = $element;
}

function solr_filter_element_ajax_response(&$form_state){
  if(empty($form_state['triggering_element'])) return;

  // AJAX
  $i = &$form_state['islandora'];
  $trigger = &$form_state['triggering_element'];
  $values = &$form_state['values'];

  if(!empty($trigger['#value']) and $trigger['#value'] == 'Apply Filter'){
    $v = str_replace(':', '\:', $values['solr_value']);
    $filter =  $values['solr_field'] . ':' . $v;
    $i['solr_filters'][$filter] = $filter;
    $i['page'] = 0;
  }
}

function solr_filter_element_return(&$form, &$form_state){
  return $form;
}

