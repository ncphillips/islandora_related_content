<?php
/**
 * @file
 */



/**
 * fedora_relationship_select
 *
 * @param $form
 * @param $form_state
 * @param $relationships
 */
function fedora_relationship_select(&$form, &$form_state, $relationships){
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
      'callback' => 'fedora_relationship_select_ajax_return',
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

/**
 * @param $form
 * @param $form_state
 * @return mixed
 * Drupal form element.
 */
function fedora_relationship_select_ajax_return($form, &$form_state){
  return $form['fedora_relationship_select']['predicate'];
}
