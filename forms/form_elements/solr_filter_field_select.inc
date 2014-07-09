<?php
/**
 * @file
 */

/**
 * @param $form
 * @param $form_state
 * @param $field
 * An array where the first value is the actual field string, and the second value
 * is user friendly label for that field.
 * @code
 * array(
 *  'RELS_EXT_hasModel_uri_mt',
 *  'Content Type'
 * );
 * @endcode
 * @param array $values
 * A key-value array where the key will be what goes into the Solr filter, and the
 * value is a user friendly label.
 * @code
 * array(
 *  'islandora:coral_cmodel' => 'Coral',
 *  'islandora:sponge_cmodel' => 'Sponge',
 * );
 * @endcode
 */
function solr_filter_field_select(&$form, &$form_state, $field, $values=array()){
  $element['solr_filter_field_label'] = array(
    '#markup' => $field[1],
  );

  $element['solr_filter_field_select'] = array(
    '#type' => 'select',
    '#options' => $values,
    '#ajax' => array(
      'callback' => 'solr_filter_field_select_ajax_return',
      'wrapper' => $form_state['islandora']['form_div_id'],
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

function solr_filter_field_select_ajax_return($form, $form_state){
  return $form;
}

