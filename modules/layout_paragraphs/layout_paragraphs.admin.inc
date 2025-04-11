<?php
/**
 * @file
 */

 /**
 * Form constructor for the Paragraphs layout configuration form.
 *
 * @param array $form
 *   An associative array containing the structure of a portion of the form.
 * @param array $form_state
 *   A keyed array containing the current state of the form.
 * @param array $field
 *   The field structure.
 * @param array $instance
 *   The field instance.
 *
 * @see layout_paragraphs_menu()
 * @see layout_paragraphs_edit_content_validate() @todo
 * @see layout_paragraphs_edit_content_submit() @todo
 * @ingroup forms
 */
function layout_paragraphs_edit_content($form, &$form_state, $field = array(), $instance = array()) {
  $default_template = (!empty($form_state['layout_paragraphs']['select_template']['layout_paragraphs_templates'])) ? $form_state['layout_paragraphs']['select_template']['layout_paragraphs_templates'] : 'mini_one_one_one';
  dpm($form_state['layout_paragraphs']['select_template']['layout_paragraphs_templates']);

  if ($instance['widget']['type'] == 'layout_paragraphs') {
    form_load_include($form_state, 'inc', 'layout_paragraphs', 'layout_paragraphs.admin');
    form_load_include($form_state, 'inc', 'layout_paragraphs', 'layout_paragraphs.context.admin');
    form_load_include($form_state, 'inc', 'layout', 'layout.admin');

    $form['#attached']['js'][] = backdrop_get_path('module', 'layout') . '/js/layout.admin.js';
    // $form['#attached']['css'][] = backdrop_get_path('module', 'mini_layouts') . '/css/mini-layouts.admin.css';

    $build['layout_paragraphs'] = array(
    '#type' => 'container',
      '#attributes' => array(
        'id' => array('ajax-layout-paragraphs-templates'),
      ),
    );
    $available_templates = $instance['widget']['settings']['available_layout_templates'];
    $all_templates = layout_flexible_template_load_all('paragraphs');
    if (!empty($available_templates)) {
      $template_options = [];
      //($instance['widget']['settings']['available_layout_templates']);
      foreach ($available_templates as $template_name) {
        if (!empty($all_templates[$template_name])) {
          $options[$template_name] = $all_templates[$template_name]->title;
          $template_options[$template_name] = $all_templates[$template_name];
        }
      }
      if (count($options) > 1) {
        $build['layout_paragraphs']['select_template'] = layout_paragraphs_select_template($build, $form_state, $template_options, $default_template);
      }
      else {
        $template_name = key($options);
        $build['layout_paragraphs']['select_template']['layout_paragraphs_template'] = array(
          '#type' => 'hidden',
          '#title' => t('Layout template'),
          '#description' => t('Which layout template should be used?'),
          '#value' => (!empty($template_name)) ? $template_name : $default_template,
          '#required' => TRUE,
        );
        $build['layout_paragraphs']['tempmarkup'] = array(
          '#markup' => '<p>' . t('Only one choice possible, so we will use it automatically.') . '</p>',
        );
      }

    }
    else {
      $build['layout_paragraphs']['tempmarkup'] = array(
        '#markup' => '<p>' . t('No layout templates are available for this field. Using default.') . '</p>',
      );
    }

    /*
    $layout = layout_load('default', TRUE, 'paragraphs');
    $layout->type = 'paragraphs';
    */
    $id = (!empty($form_state[$instance['entity_type']]->id())) ? '.' . $form_state[$instance['entity_type']]->id() : '';
    $config = array(
      'is_new' => TRUE,
      'layout_template' => $default_template,
      'type' => 'paragraphs',
      'is_full_page' => FALSE,
      'name' => $instance['entity_type'] . '.' . $instance['bundle'] . '.' . $instance['field_name'] . $id,
    );
    $layout = new Layout($config);
    $layout_renderer = layout_get_renderer_info('paragraphs_editor');
    $renderer = new LayoutRendererEditorParagraphs($layout, $layout_renderer);
    $build['layout_paragraphs']['layout']['#markup'] = '<div class="messages info">TBD: Render Layout Here</div>' . $renderer->render();

    return $build;
  }
}

/**
 * Submit handler for layout_paragraphs_edit_content().
 */
function layout_paragraphs_edit_content_submit($form, &$form_state) {
  $field_name = $form_state['field']['field_name'];
  $instance = $form_state['field']['instance'];
  $field = $form_state['field'];
  $field['settings']['available_layout_templates'] = $form_state['values']['layout_paragraphs_template'];

}

/**
 * Select a Layout Paragraphs template from the options.
 */
function layout_paragraphs_select_template($build, &$form_state, $template_options, $default = 'default') {
  $build['#attached']['js'][] = backdrop_get_path('module', 'layout') . '/js/layout.admin.js';
  $build['#attached']['css'][] = backdrop_get_path('module', 'layout') . '/css/layout.admin.css';

  foreach ($template_options as $template_option) {
    $pause = 1;
  }

  $build['#attributes'] = array(
    'class' => array('layout-paragraphs-settings-form'),
  );

  $build['layout_paragraphs_templates'] = array(
    '#title' => t('Layout template'),
    '#type' => 'radios',
    '#default_value' => $default,
    '#options' => array(),
    '#wrapper_attributes' => array(
      'class' => array('clearfix', 'layout-options'),
    ),
    '#after_build' => array(
      'layout_paragraphs_select_template_after_build',
    ),
    '#required' => TRUE,
  );

  foreach ($template_options as $template_name => $template_info) {
    $build['layout_paragraphs_templates']['#options'][$template_name] = $template_info->title;
    if ($template_name == $default) {
      // $build['layout_paragraphs_templates']['#options'][$template_name]['#attributes']['data-default'] = 'default';
    }
    // $build['layout_paragraphs_template'][$template_name]['#description'] = theme('layout_paragraphs_template_option', array('template_info' => $template_info));
    $build['layout_paragraphs_templates'][$template_name]['#description'] = 'hi'; // $template_info->preview;
  }

  $build['submit'] = array(
    '#type' => 'submit',
    '#submit' => array(
      'layout_paragraphs_select_template_change',
    ),
    '#value' => t('Change layout template'),
    '#ajax' => array(
      'callback' => 'layout_paragraphs_select_template_change_ajax',
      'wrapper' => 'ajax-layout-paragraphs-templates',
      'effect' => 'fade',
    ),
  );

  return $build;
}

function layout_paragraphs_select_template_after_build($element, $form_state) {
  $default_template = $element['#default_value'];
  $element[$default_template]['#attributes']['data-default'] = 'true';
  return $element;
}

function layout_paragraphs_select_template_change_ajax($form, &$form_state) {
  return $form['field_new_para_field']['und']['layout_paragraphs'];
}

/**
 * Ajax handler for layout_paragraphs_select_template() to change the active
 * Layout Paragraphs template.
 *
 * @param [type] $form
 * @param [type] $form_state
 * @return void
 */
function layout_paragraphs_select_template_change($form, &$form_state) {
  $new_template_name = $form_state['values']['field_new_para_field']['und']['layout_paragraphs']['select_template']['layout_paragraphs_templates'];
  $form_state['layout_paragraphs']['select_template']['layout_paragraphs_templates'] = $new_template_name;
  $form_state['rebuild'] = TRUE;
  
}

/**
 * Administrative form for Layout Paragraphs settings.
 */
function layout_paragraphs_settings_page($form, &$form_state) {
  $header = array(
    'layout_paragraphs_template_info' => array(
      'data' => t('Template')),
    'type' => array(
      'data' => t('Type'),
      'sort' => 'desc',
      'class' => array(RESPONSIVE_PRIORITY_LOW),
    ),
    'operations' => t('Operations'),
  );

  backdrop_set_title(t('Layout Paragraphs settings'));
  $excluded = config_get('paragraphs.settings', 'excluded_layouts');

  $rows = array();
  $options = array();
  $default = array();
  $form_state['hidden_templates'] = array();
  $all_templates = layout_flexible_template_load_all('paragraphs');

  foreach ($all_templates as $template_name => $template) {
    // Build the default values list.
    $is_default = FALSE;
    (empty($template->hidden)) ? $template->hidden = 0 : $template->hidden = 1;
    $template_info = array(
      'preview' => (!empty($template->preview)) ? $template->preview : '',
      'title' => $template->title,
      'name' => $template_name,
    );
    if (is_array($excluded) && !in_array($template_name, $excluded)) {
      $is_default = $default[$template_name] = TRUE;
    }

    // Build the list of possible options, either enabled or not hidden.
    if ($is_default || !$template->hidden) {
      $rows[$template_name] = array(
        'layout_paragraphs_template_info' => theme('layout_paragraphs_template_info', array(
          'template_info' => $template_info,
        )),
        'type' => t('Paragraphs'),
      );
      $rows[$template_name]['operations']['data'] = array(
        '#type' => 'dropbutton',
        '#links' => _layout_paragraphs_settings_get_group_operations($template_info, $is_default),
      );
    }
    // For disabled and hidden templates, save the fact they are disabled.
    elseif ($template->hidden) {
      $form_state['hidden_templates'][] = $template_name;
    }
  }

  $form['#attached']['css'][] = backdrop_get_path('module', 'layout') . '/css/layout.admin.css';
  // Empty table header for enable/disable is confusing (below); converted to
  // standard table for now.
  /*
  $form['templates'] = array(
    '#type' => 'tableselect',
    '#js_select' => FALSE,
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('There are no Layout Paragraphs templates.'),
    '#default_value' => $default,
    '#attributes' => array('class' => array('layout-list')),
  );
  */
  $form['templates'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no Layout Paragraphs templates.'),
    '#attributes' => array('class' => array('layout-list')),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Validation for Layout Paragraphs settings page.
 *
 * Ensures that active layouts are not disabled while in use.
 */
function layout_paragraphs_settings_page_validate(&$form, &$form_state) {
  $options = array_keys($form_state['values']['templates']);
  $included = array_values($form_state['values']['templates']);
  $excluded = array_unique(array_diff($options, $included));
  $form_state['excluded_layouts'] = array_values(array_merge($excluded, $form_state['hidden_templates']));

  // Ensure all layouts are not disabled.
  if (empty($form_state['values']['templates'])) {
    form_set_error('templates', t('At least one layout template must be enabled.'));
  }

  // Check which layout templates are in use.
  // @todo This can just load all Paragraphs fields that use the Layouts widget
  // and check which templates are in use from the settings there.
  $all_templates = layout_flexible_template_load_all('paragraphs');
  foreach ($all_templates as $template_name => $template) {
    if (in_array($template_name, $excluded)) {
      form_set_error('templates][' . $template_name, t('The "@layout" layout template is currently in use and may not be disabled.', array('@layout' => $template->title)));
    }
  }
}

/**
 * Submit handler for Layout Paragraphs settings page.
 */
function layout_paragraphs_settings_page_submit($form, &$form_state) {
  if (!empty($form_state['excluded_layouts'])) {
    config_set('layout_paragraphs.settings', 'excluded_layouts', $form_state['excluded_layouts']);
  }
  backdrop_set_message(t('Your configuration has been saved.'));
}


  /**
   * Given a path, return a list of operations for a group of layouts.
   */
function _layout_paragraphs_settings_get_group_operations($template_info, $is_default) {
  $links = array();

  if (user_access('administer paragraphs layouts')) {
    $links['configure'] = array(
      'title' => t('Configure regions'),
      'href' => 'admin/structure/paragraphs/layouts/' . $template_info['name'] . '/configure',
    );
    $links['edit'] = array(
      'title' => t('Configure name'),
      'href' => 'admin/structure/paragraphs/layouts/' . $template_info['name'] . '/edit',
    );
    if (module_exists('config') && user_access('synchronize configuration')) {
      $links['export'] = array(
        'title' => t('Export'),
        'href' => 'admin/config/development/configuration/single/export',
        'query' => array(
          'group' => t('Layout Paragraphs'),
          'name' => 'layout.paragraphs.' . $template_info['name'],
        ),
      );
    }
    if (!$is_default) {
      $links['delete'] = array(
        'title' => t('Delete'),
        'href' => 'admin/structure/paragraphs/layouts/' . $template_info['name'] . '/delete',
      );
    }
    if (empty($template_info['hidden'])) {
      $links['disable'] = array(
        'title' => t('Disable'),
        'href' => 'admin/structure/layouts/settings/toggle/' . $template_info['name'] . '/disable',
        'query' => array('token' => backdrop_get_token('layout-template-' . $template_info['name'])),
      );
    }
    else {
      $links['enable'] = array(
        'title' => t('Enable'),
        'href' => 'admin/structure/layouts/settings/toggle/' . $template_info['name'] . '/enable',
        'query' => array('token' => backdrop_get_token('layout-template-' . $template_info['name'])),
      );
    }

    return $links;
  }
}

/**
 * Form to add a flexible Layout Paragraphs template.
 *
 * @ingroup forms
 */
function layout_paragraphs_template_settings_add_form() {
  $config = array(
    'is_new' => TRUE,
    'type' => 'paragraphs',
    'is_full_page' => FALSE,
  );
  $layout_paragraphs_template = new LayoutFlexibleTemplate($config);
  backdrop_set_title(t('Add template'));
  return backdrop_get_form('layout_paragraphs_template_settings_edit_form', $layout_paragraphs_template);
}

/**
 * Determine whether a layout paragraph template name already exists.
 */
function _layout_paragraphs_template_exists($template_name) {
  $template = layout_flexible_template_load($template_name, 'paragraphs');
  return !empty($template);
}

/**
 * Form to configure a flexible Layout Paragraphs template.
 *
 * @param LayoutFlexibleTemplate $layout_paragraphs_template
 *   The loaded flexible template object.
 *
 * @ingroup forms
 */
function layout_paragraphs_template_settings_edit_form($form, &$form_state, LayoutFlexibleTemplate $layout_paragraphs_template) {
  $form_state['flexible_template'] = $layout_paragraphs_template;
  $template_exists = layout_flexible_template_load($layout_paragraphs_template->name, 'paragraphs');

  $form['name'] = array(
    '#title' => t('Template name'),
    '#type' => 'textfield',
    '#maxlength' => 128,
    '#default_value' => $layout_paragraphs_template->title,
    '#required' => TRUE,
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 21,
    '#default_value' => $layout_paragraphs_template->name,
    '#disabled' => ($template_exists && empty($layout_paragraphs_template->is_new)),
    '#machine_name' => array(
      'exists' => '_layout_paragraphs_template_exists',
    ),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#default_value' => $layout_paragraphs_template->description,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#submit' => array(
      'layout_paragraphs_template_settings_form_submit',
    ),
    '#value' => t('Save and configure'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#validate' => array(),
    '#limit_validation_errors' => array(array('name', 'machine_name')),
    '#submit' => array(
      'layout_paragraphs_template_settings_form_cancel',
    ),
  );

  return $form;
}

/**
 * Submit handler that cancels the flexible Layout Paragraphs template form.
 *
 * @see layout_paragraphs_template_settings_edit_form()
 */
function layout_paragraphs_template_settings_form_cancel($form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts';
}

/**
 * Submit handler that saves a flexible Layout Paragraphs template.
 *
 * @see layout_paragraphs_template_settings_edit_form()
 */
function layout_paragraphs_template_settings_form_submit($form, &$form_state) {
  /* @var LayoutFlexibleTemplate $layout_paragraphs_template */
  $layout_paragraphs_template = $form_state['flexible_template'];
  $layout_paragraphs_template->name = $form_state['values']['machine_name'];
  $layout_paragraphs_template->title = $form_state['values']['name'];
  $layout_paragraphs_template->description = $form_state['values']['description'];
  $layout_paragraphs_template->type = 'paragraphs';
  $layout_paragraphs_template->is_full_page = FALSE;
  unset($layout_paragraphs_template->is_new);
  $layout_paragraphs_template->preview = _layout_paragraphs_template_generate_svg($layout_paragraphs_template);

  $layout_paragraphs_template->save();

  layout_flexible_tempstore_set($layout_paragraphs_template, 'paragraphs');
  cache()->delete('layout:paragraphs:config');
  backdrop_set_message(t('Layout paragraphs template "@title" saved.', array('@title' => $layout_paragraphs_template->title)));
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts/' . $form_state['values']['machine_name'] . '/configure';
}

/**
 * Form to manage adding/removing rows to a flexible Layout Paragraphs template.
 *
 * @param LayoutFlexibleTemplate $layout_paragraphs_template
 *   The template configuration being edited.
 *
 * @ingroup forms
 */
function layout_paragraphs_template_configure_form($form, &$form_state, LayoutFlexibleTemplate $layout_paragraphs_template) {
  form_load_include($form_state, 'inc', 'layout', 'layout.admin');
  form_load_include($form_state, 'inc', 'layout', 'layout.flexible');
  form_load_include($form_state, 'inc', 'layout_paragraphs', 'layout_paragraphs.admin');

  $form['help'] = array(
    '#type' => 'help',
    '#markup' => t('This page allows you to add or remove rows, and change the order of rows by dragging them up and down. You can also change the number of regions within each row, and adjust the width of each region.'),
  );

  $messages = array();
  $form['messages'] = array(
    '#theme' => 'status_messages',
    '#messages' => $messages,
    '#weight' => -100,
    // Prefix/suffix used to identify in AJAX requests.
    '#prefix' => '<div id="layout-messages">',
    '#suffix' => '</div>',
  );

  $form_state['flexible_template'] = &$layout_paragraphs_template;
  $layout_paragraphs_template_name = $layout_paragraphs_template->name;
  $form_state['flexible_template_name'] = $layout_paragraphs_template_name;
  $form_state['flexible_template_type'] = 'paragraphs';

  $config = array(
    'is_new' => TRUE,
    'type' => 'paragraphs',
    'is_full_page' => FALSE,
    'layout_template' => $layout_paragraphs_template_name,
  );
  $layout = new Layout($config);

  backdrop_set_title($layout_paragraphs_template->title);


  if (empty($form_state['flexible_template'])) {
    return array();
  }

  _layout_include_files();

  $renderer_info = array(
    'class' => 'LayoutRendererParagraphs',
    'module' => 'layout_paragraphs',
    'name' => 'paragraphs',
    'path' => backdrop_get_path('module', 'layout_paragraphs') . '/plugins/renderers',
  );
  $renderer = new LayoutRendererParagraphs($layout, $renderer_info);

  // $renderer = layout_create_renderer('paragraphs', $layout);

  $form['content'] = array(
    '#type' => 'item',
    '#id' => 'layout-flexible-content',
  );
  $form['content']['display'] = array(
    '#markup' => $renderer->render(),
  );

  $form['content']['row_positions'] = array(
    // Use 'hidden' instead of 'value' so the JS can access it.
    '#type' => 'hidden',
    '#default_value' => implode(',', array_keys($layout_paragraphs_template->rows)),
  );

  $form['content']['add_row'] = array(
    '#type' => 'submit',
    '#value' => t('Add row'),
    '#attributes' => array('class' => array('layout-flexible-add-row', 'button-secondary')),
    '#submit' => array(
      'layout_paragraphs_template_add_row',
    ),
    '#ajax' => array(
      'callback' => 'layout_ajax_form_open_dialog',
    ),
  );
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save layout template'),
    '#submit' => array(
      'layout_paragraphs_template_configure_form_submit',
    ),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array(
      'layout_paragraphs_template_configure_form_cancel',
    ),
  );

  return $form;
}

/**
 * Handler for layout_paragraphs_template_configure_form() to add a new row.
 */
function layout_paragraphs_template_add_row($form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts/' . $form_state['flexible_template_name'] . '/row/add/select-widths';
}

/**
 * Submit handler that cancels in-progress changes.
 *
 * @see layout_paragraphs_template_configure_form()
 */
function layout_paragraphs_template_configure_form_cancel($form, &$form_state) {
  tempstore_clear('layout.paragraphs', $form_state['flexible_template_name']);
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts';
}

/**
 * Submit handler that saves the configured flexible Layout Paragraphs template.
 *
 * @see layout_paragraphs_template_configure_form()
 */
function layout_paragraphs_template_configure_form_submit($form, &$form_state) {
  // Load from tempstore rather than using $form_state, in the event a row
  // has been removed in an AJAX request.
  $layout_paragraphs_template = layout_flexible_tempstore_load($form_state['flexible_template_name'], 'paragraphs');
  $positions = array();

  if (!empty($form_state['values']['row_positions'])) {
    $rows = array_filter(explode(',', $form_state['values']['row_positions']));
    foreach ($rows as $position) {
      if (isset($layout_paragraphs_template->rows[$position])) {
        $positions[$position] = $layout_paragraphs_template->rows[$position];
      }
    }
  }

  $layout_paragraphs_template->rows = $positions;
  $layout_paragraphs_template->preview = _layout_paragraphs_template_generate_svg($layout_paragraphs_template);
  $layout_paragraphs_template->save();

  tempstore_clear('layout.paragraphs', $form_state['flexible_template_name']);
  cache()->delete('layout:paragraphs:config');
  backdrop_set_message(t('Layout Paragraphs template "@title" saved.', array('@title' => $layout_paragraphs_template->title)));
}

/**
 * Form to select the region widths for a flexible Layout Paragraphs template.
 *
 * @param LayoutFlexibleTemplate $layout_paragraphs_template
 *   The loaded flexible template object.
 * @param string $original_row
 *   The row above or below which a new row is being inserted.
 *
 * @ingroup forms
 */
function layout_paragraphs_template_region_style_select($form, &$form_state, LayoutFlexibleTemplate $layout_paragraphs_template, $original_row) {
  $new_row = ($original_row == 'add');
  form_load_include($form_state, 'inc', 'layout', 'layout.admin');
  form_load_include($form_state, 'inc', 'layout', 'layout.flexible');
  form_load_include($form_state, 'inc', 'layout_paragraphs', 'layout_paragraphs.admin');
  $form_state['flexible_template_name'] = $layout_paragraphs_template->name;
  $form_state['original_row'] = $original_row;

  $row_styles = layout_flexible_row_styles();
  $count_options = array();
  foreach ($row_styles as $name => $row_style) {
    $count_options[$row_style['region_count']] = $row_style['region_count'];
  }

  $row_style_in_use = 'region_12';
  $default_region_count = 1;
  if (!$new_row) {
    if (isset($form_state['values']['region_style'])) {
      $row_style_in_use = $form_state['values']['region_style'];
    }
    else {
      $row_style_in_use = $layout_paragraphs_template->rows[$original_row]['contains'];
    }
    $default_region_count = $row_styles[$row_style_in_use]['region_count'];
  }
  $form['region_count'] = array(
    '#title' => t('Number of regions'),
    '#type' => 'radios',
    '#default_value' => $default_region_count,
    '#parents' => array('region_count'),
    '#options' => $count_options,
    '#ajax' => array(
      'callback' => 'layout_paragraphs_template_edit_region_style_ajax',
      'wrapper' => 'row_settings',
      'trigger_as' => array('name' => 'region_count_update'),
    ),
  );
  $form['region_count_update'] = array(
    '#type' => 'submit',
    '#value' => t('Update regions'),
    '#attributes' => array('class' => array('js-hide')),
    '#name' => 'region_count_update',
    '#validate' => array(),
    '#submit' => array('layout_paragraphs_template_edit_region_style_submit'),
    '#ajax' => array(
      'callback' => 'layout_paragraphs_template_edit_region_style_ajax',
      'wrapper' => 'row_settings',
    ),
  );

  $form['row_settings'] = array(
    '#type' => 'container',
    '#id' => 'row_settings',
    '#parents' => array('row_settings'),
  );

  $region_count = isset($form_state['values']['region_count']) ? $form_state['values']['region_count'] : $default_region_count;

  $options = array();
  foreach ($row_styles as $name => $row_style) {
    if ($row_style['region_count'] == $region_count) {
      $options[$name] = theme('layout_flexible_template_style_option', array(
        'row_style' => $row_style,
      ));
    }
  }

  if (empty($options)) {
    $options['region_12'] = '12:0';
  }

  $default_region_style = $row_style_in_use;
  if (!isset($options[$row_style_in_use])) {
    $default_region_style = key($options);
  }
  $form['row_settings']['region_style'] = array(
    '#title' => t('Region widths'),
    '#type' => 'radios',
    '#default_value' => $default_region_style,
    '#options' => $options,
  );

  $form['submit_region_style'] = array(
    '#type' => 'submit',
    '#value' => t('Choose region widths'),
    '#attributes' => array(
      'class' => array('js-hide'),
    ),
    '#submit' => array(
      'layout_paragraphs_template_region_count_load',
    ),
  );

  $submit_button_value = $new_row ? t('Continue') : t('Save region widths');
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'submit',
    '#value' => $submit_button_value,
    '#attributes' => array('class' => array('layout-title-button')),
    '#submit' => array(
      'layout_paragraphs_template_region_style_select_submit',
    ),
    '#ajax' => array(
      'callback' => 'layout_ajax_form_open_dialog',
    ),
  );
  if (isset($layout_paragraphs_template->rows[$original_row]['in_progress'])) {
    $cancel_submit_callback = 'layout_paragraphs_template_region_style_select_cancel';
    $cancel_ajax_callback = 'layout_ajax_form_open_dialog';
  }
  else {
    $cancel_submit_callback = 'layout_paragraphs_template_cancel';
    $cancel_ajax_callback = 'layout_paragraphs_template_cancel_ajax';
  }
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#validate' => array(),
    '#submit' => array(
      $cancel_submit_callback,
    ),
    '#ajax' => array(
      'callback' => $cancel_ajax_callback,
    ),
  );

  return $form;
}

/**
 * Submit handler that cancels the region style selection form.
 *
 * @see layout_paragraphs_template_region_style_select().
 */
function layout_paragraphs_template_cancel($form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts/' . $form_state['flexible_template_name'] . '/configure';
}

/**
 * AJAX handler to cancels the region style selection form.
 *
 * @see layout_paragraphs_template_region_style_select().
 */
function layout_paragraphs_template_cancel_ajax($form, $form_state) {
  $commands = array();
  $commands[] = ajax_command_close_modal_dialog();

  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Submit handler that cancels the region style selection form.
 *
 * @see layout_paragraphs_template_region_style_select().
 */
function layout_paragraphs_template_region_style_select_cancel($form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts/' . $form_state['flexible_template_name'] . '/row/' . $form_state['original_row'] . '/configure';
}

/**
 * Submit handler that saves the region style selection form.
 *
 * @see layout_paragraphs_template_region_style_select().
 */
function layout_paragraphs_template_region_style_select_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts/' . $form_state['flexible_template_name'] . '/row/' . $form_state['original_row'] . '/configure/' . $form_state['values']['region_style'];
}

/**
 * Form to add or edit a row on a flexible Layout Paragraphs template.
 *
 * @param LayoutFlexibleTemplate $layout_paragraphs_template
 *   The loaded flexible template object.
 * @param string $original_row
 *   The row above or below which a new row is being inserted.
 * @param string $region_style
 *   The selected region style.
 *
 * @ingroup forms
 */
function layout_paragraphs_template_edit_row_form($form, &$form_state, LayoutFlexibleTemplate $layout_paragraphs_template, $original_row, $region_style = NULL) {
  form_load_include($form_state, 'inc', 'layout', 'layout.flexible');
  form_load_include($form_state, 'inc', 'layout', 'layout.admin');
  form_load_include($form_state, 'inc', 'layout_paragraphs', 'layout_paragraphs.admin');
  $form_state['flexible_template'] = &$layout_paragraphs_template;
  $layout_paragraphs_template_name = $layout_paragraphs_template->name;

  $form_state['flexible_template_name'] = $layout_paragraphs_template_name;
  $form_state['original_row'] = $original_row;

  $row_data = array();
  if ($original_row != 'add') {
    if (isset($layout_paragraphs_template->rows[$original_row]['in_progress'])) {
      $row_data = $layout_paragraphs_template->rows[$original_row]['in_progress'];
    }
    else {
      $row_data = $layout_paragraphs_template->rows[$original_row];
    }
  }

  $region_style = $region_style ? $region_style : $row_data['contains'];
  $form_state['region_style'] = $region_style;

  $row_styles = layout_flexible_row_styles();
  $selected_style = $row_styles[$region_style]['name'];
  $region_count = $row_styles[$region_style]['region_count'];

  if ($original_row == 'add') {
    backdrop_set_title(t('Add new row'));
  }
  else {
    backdrop_set_title(t('Configure row !original_row', array('!original_row' => $original_row)));
  }

  $form['region_style'] = array(
    '#type' => 'item',
    '#title' => t('Selected region widths'),
    '#markup' => $selected_style,
  );

  if ($original_row != 'add') {
    $form['change_region_style'] = array(
      '#type' => 'submit',
      '#value' => t('Change region widths'),
      '#attributes' => array('class' => array('layout-link-button')),
      '#submit' => array(
        'layout_paragraphs_template_change_region_style',
      ),
      '#ajax' => array(
        'callback' => 'layout_ajax_form_open_dialog',
      ),
    );
  }

  $form['region_names'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regions'),
    '#collapsed' => $original_row != 'add',
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );

  $form_state['last_region_number'] = layout_paragraphs_template_get_last_region_number($layout_paragraphs_template);
  for ($i = 0; $i < $region_count; $i++) {
    $form['region_names']['region_' . $i] = array(
      '#type' => 'container',
    );
    if (isset($row_data['region_names']['region_' . $i]['label'])) {
      $region_name = $row_data['region_names']['region_' . $i]['label'];
    }
    $form['region_names']['region_' . $i]['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Region name') . ' ' . ($i + 1),
      '#default_value' => !empty($region_name) ? $region_name : t('Region') . ' ' . ($form_state['last_region_number'] + $i + 1),
    );
    $form['region_names']['region_' . $i]['name'] = array(
      '#type' => 'machine_name',
      '#description' => t('A machine name to be used as the default CSS class. It must only contain uppercase letters, lowercase letters, numbers, hyphens and underscores.'),
      '#default_value' => !empty($row_data['region_names']['region_' . $i]['name']) ? $row_data['region_names']['region_' . $i]['name'] : '',
      '#maxlength' => 255,
      '#field_prefix' => '<span dir="ltr">l-region--',
      '#field_suffix' => '</span>&lrm;',
      '#size' => 15,
      '#disabled' => FALSE,
      '#machine_name' => array(
        'exists' => '_layout_paragraphs_template_region_name_exists',
        'source' => array('region_names', 'region_' . $i, 'label'),
        'standalone' => TRUE,
        'label' => t('Name'),
        'replace_pattern' => '[^a-z0-9-]+',
        'replace' => '-',
      ),
    );
    if (isset($row_data['region_names']['region_' . $i]['region_class_enable'])){
      $region_class_enable = $row_data['region_names']['region_' . $i]['region_class_enable'];
    }
    $form['region_names']['region_' . $i]['region_class_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add CSS classes'),
      '#default_value' => !empty($region_class_enable) ? $region_class_enable : 0,
    );
    if (isset($row_data['region_names']['region_' . $i]['classes'])) {
      $classes = $row_data['region_names']['region_' . $i]['classes'];
    }
    $form['region_names']['region_' . $i]['classes'] = array(
      '#type' => 'textfield',
      '#title' => t('Additional region CSS classes'),
      '#description' => t('Separate multiple classes with spaces.'),
      '#default_value' => !empty($classes) ? $classes : '',
      '#states' => array(
        'visible' => array(
          ':input[name="region_names[region_' . $i . '][region_class_enable]"]' => array('checked' => TRUE),
        ),
      ),
    );
  }

  $form['region_styles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Row Style'),
    '#collapsed' => $original_row != 'add',
    '#collapsible' => TRUE,
  );

  $options = array(
    'div' => 'DIV',
    'nav' => 'NAV',
    'aside' => 'ASIDE',
    'section' => 'SECTION',
    'header' => 'HEADER',
    'footer' => 'FOOTER',
    'main' => 'MAIN',
  );
  $form['region_styles']['element'] = array(
    '#title' => t('Row wrapper tag'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => isset($row_data['element']) ? $row_data['element'] : '',
  );

  $form['region_styles']['container'] = array(
    '#title' => t('Row width behavior'),
    '#type' => 'radios',
    '#options' => array(
      'container' => t('Fixed maximum width'),
      'container_fluid' => t('Fluid width'),
      'no_container' => t('Full width'),
    ),
    '#default_value' => isset($row_data['container']) ? $row_data['container'] : 'container_fluid',
  );
  $form['region_styles']['container']['container']['#description'] = t('Adds the <code>container</code> class to the row.');
  $form['region_styles']['container']['container_fluid']['#description'] = t('Adds the <code>container-fluid</code> class to the row (no <code>max-width</code>, but <code>padding</code>).');
  $form['region_styles']['container']['no_container']['#description'] = t('No container-related classes added to the row (e.g. for Hero blocks).');

  $form['region_styles']['row_classes'] = array(
    '#title' => t('Additional row CSS classes'),
    '#type' => 'textfield',
    '#default_value' => isset($row_data['classes']) ? $row_data['classes'] : '',
    '#description' => t('Separate multiple classes with spaces.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'submit',
    '#value' => t('Save configuration'),
    '#attributes' => array('class' => array('layout-title-button')),
    '#validate' => array(
      'layout_paragraphs_template_edit_row_validate',
    ),
    '#submit' => array(
      'layout_paragraphs_template_edit_row_form_submit',
    ),
    '#ajax' => array(
      'callback' => 'layout_paragraphs_template_edit_row_ajax',
    ),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#validate' => array(),
    '#submit' => array(
      'layout_paragraphs_template_cancel',
    ),
    '#ajax' => array(
      'callback' => 'layout_paragraphs_template_cancel_ajax',
    ),
  );

  return $form;
}

/**
 * Callback: Check if machine_name is unique.
 *
 * Even though callback is required for the Form API machine_name field type,
 * the region classes are not required to be unique. It will accept any
 * machine_name.
 *
 * @param $machine_name
 *   The machine name for a region.
 *
 * @return
 *   Whether or not the region name is taken.
 *
 * @see layout_paragraphs_template_edit_row_form()
 * @see form_validate_machine_name()
 */
function _layout_paragraphs_template_region_name_exists($machine_name, $element, $form_state) {
  $layout_paragraphs_template = layout_flexible_tempstore_load($form_state['flexible_template_name'], 'paragraphs');
  foreach ($layout_paragraphs_template->rows as $row) {
    foreach ($row['region_names'] as $region_name) {
      if ($region_name['name'] == $machine_name) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
 * Returns the last number of all regions named "Region X" where X is a number.
 */
function layout_paragraphs_template_get_last_region_number(LayoutFlexibleTemplate $layout_paragraphs_template, $form_state = NULL) {
  $last = 0;
  $region_names = array();
  if ($form_state) {
    $regions = array_filter($form_state['values']['region_names']);
    $last = $form_state['last_region_number'];
    foreach ($regions as $key => $region) {
      $region_names[] = $region['label'];
    }
  }
  else {
    foreach ($layout_paragraphs_template->rows as $row) {
      foreach ($row['region_names'] as $key => $region) {
        $region_names[] = $region['label'];
      }
    }
  }

  foreach ($region_names as $region) {
    $str = explode('Region ', $region);
    if (!empty($str[1]) && is_numeric($str[1]) && $str[1] > $last) {
      $last = $str[1];
    }
  }

  return $last;
}

/**
 * Submit handler for the change region style button on the
 * layout_paragraphs_template_edit_row_form().
 */
function layout_paragraphs_template_change_region_style($form, &$form_state) {
  $old_row = array(
    'contains' => $form_state['region_style'],
    'element' => $form_state['values']['element'],
    'classes' => $form_state['values']['row_classes'],
    'container' => $form_state['values']['container'],
    'region_names' => $form_state['values']['region_names'],
  );
  $form_state['flexible_template']->rows[$form_state['original_row']]['in_progress'] = $old_row;

  layout_flexible_tempstore_set($form_state['flexible_template'], 'paragraphs');

  $form_state['redirect'] = 'admin/structure/paragraphs/layouts/' . $form_state['flexible_template_name'] . '/row/' . $form_state['original_row'] . '/select-widths';
}

/**
 * Submit handler on the hidden button to change region widths on
 * layout_paragraphs_template_edit_row_form() if AJAX is disabled.
 */
function layout_paragraphs_template_region_count_load(&$form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Validation handler for layout_paragraphs_template_edit_row_form().
 */
function layout_paragraphs_template_edit_row_validate($form, &$form_state) {
  $last_region_number = layout_paragraphs_template_get_last_region_number($form_state['flexible_template'], $form_state);

  foreach ($form_state['values']['region_names'] as $key => $region) {
    if (empty($region)) {
      $number = str_replace('region_', '', $key);
      $form_state['values']['region_names'][$key]['label'] = t('Region') . ' ' . ($last_region_number + $number + 1);
      $last_region_number++;
    }
  }
}

/**
 * Submit handler for layout_paragraphs_template_edit_row_form().
 */
function layout_paragraphs_template_edit_row_form_submit($form, &$form_state) {
  form_load_include($form_state, 'inc', 'layout', 'layout.admin');
  form_load_include($form_state, 'inc', 'layout', 'layout.paragraphs');
  /* @var LayoutFlexibleTemplate $layout_paragraphs_template */
  $layout_paragraphs_template = $form_state['flexible_template'];
  $original_row = $form_state['original_row'];

  $new_row = array(
    'contains' => $form_state['region_style'],
    'element' => $form_state['values']['element'],
    'classes' => $form_state['values']['row_classes'],
    'container' => $form_state['values']['container'],
    'region_names' => $form_state['values']['region_names'],
  );

  if ($original_row == 'add') {
    $uuid = new Uuid();
    $original_row = $uuid->generate();
  }

  $layout_paragraphs_template->rows[$original_row] = $new_row;

  $form_state['flexible_template'] = $layout_paragraphs_template;

  $form_state['ajax_rebuild_form'] = 'layout_paragraphs_template_configure_form';
  $form_state['ajax_rebuild_args'] = array($layout_paragraphs_template);
  $form_state['ajax_update'] = array('content');
  layout_flexible_tempstore_set($layout_paragraphs_template, 'paragraphs');
  //@todo: Can we make this row appear automatically?
  layout_paragraphs_reset_caches();
  cache()->delete('layout:paragraphs:config');
  if (!backdrop_is_ajax()) {
    $form_state['redirect'] = 'admin/structure/paragraphs/layouts/' . $form_state['flexible_template_name'] . '/configure';
  }
}

/**
 * AJAX callback to change the region count.
 *
 * @see layout_paragraphs_template_region_style_select()
 */
function layout_paragraphs_template_edit_region_style_ajax($form, &$form_state) {
  tempstore_clear('layout.paragraphs', $form_state['flexible_template_name']);
  return $form['row_settings'];
}

/**
 * Submit callback to change the region count.
 *
 * @see layout_paragraphs_template_region_style_select()
 */
function layout_paragraphs_template_edit_region_style_submit($form, &$form_state) {
  // Remove the current region style when switching between region counts.
  $form_state['input']['region_style'] = NULL;
  $form_state['rebuild'] = TRUE;
}

/**
 * AJAX callback on the submit button for editing a layout row.
 *
 * @see layout_paragraphs_template_edit_row_form().
 */
function layout_paragraphs_template_edit_row_ajax($form, $form_state) {
  $commands = array();
  // Display error messages in the form if any.
  if (form_get_errors()) {
    $html = '';
    $html .= theme('status_messages');
    $html .= backdrop_render($form);
    $title = isset($form['#title']) ? $form['#title'] : backdrop_get_title();
    $commands[] = ajax_command_open_modal_dialog($title, $html, array('dialogClass' => 'layout-dialog'));
  }
  else {
    $commands[] = ajax_command_close_modal_dialog();

    $update_ajax = layout_ajax_form_update($form, $form_state);
    $commands = array_merge($commands, $update_ajax['#commands']);
  }

  // Update the messages area.
  $commands[] = ajax_command_remove('#messages');
  $commands[] = ajax_command_html('#layout-messages', theme('status_messages'));

  // Delete cache to ensure the new row is displayed.
  cache()->delete('layout:paragraphs:config');

  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Menu callback to delete a row from a flexible Layout Paragraphs template.
 *
 * @param LayoutFlexibleTemplate $layout_paragraphs_template
 *   The loaded flexible template object.
 * @param string $original_row
 *   The row above or below which a new row is being inserted.
 *
 * @ingroup forms
 */
function layout_paragraphs_template_delete_row(LayoutFlexibleTemplate $layout_paragraphs_template, $original_row) {
  if (!isset($_GET['token']) || !backdrop_valid_token($_GET['token'], 'layout-region-' . $original_row)) {
    return MENU_ACCESS_DENIED;
  }

  $commands = array();
  $layout_paragraphs_template_name = $layout_paragraphs_template->name;

  unset($layout_paragraphs_template->rows[$original_row]);
  layout_flexible_tempstore_set($layout_paragraphs_template, 'paragraphs');

  if (backdrop_is_ajax()) {
    $commands[] = ajax_command_remove('#flexible-row--' . $original_row);

    return array(
      '#type' => 'ajax',
      '#commands' => $commands,
    );
  }
  else {
    backdrop_set_message(t('Row "@title" removed.', array('@title' => $layout_paragraphs_template->title)));
    backdrop_goto('admin/structure/paragraphs/layouts/' . $layout_paragraphs_template_name . '/configure');
    return array();
  }
}

/**
 * Menu callback; Delete a flexible template.
 *
 * @ingroup forms
 */
function layout_paragraphs_template_delete_form($form, &$form_state, LayoutFlexibleTemplate $layout_paragraphs_template) {
  $form_state['flexible_template'] = &$layout_paragraphs_template;
  $question = t('Delete flexible template @title?', array('@title' => $layout_paragraphs_template->title));
  $description = t('This action cannot be undone.');
  backdrop_set_title($question, PASS_THROUGH);

  // Check if this template is in use.
  // @todo: Adjust this check for Layout Paragraphs usage
  $errors = array();
  foreach (layout_load_all() as $layout) {
    if ($layout->layout_template == $layout_paragraphs_template->name) {
      $errors[] = check_plain($layout->title);
    }
  }

  if (!empty($errors)) {
    backdrop_set_message(t('The "@title" layout template is currently in use in the following layouts and may not be deleted: !list', array('@title' => $layout_paragraphs_template->title, '!list' => theme('item_list', array('items' => $errors)))), 'error');
  }
  else {
    $form['description'] = array(
      '#markup' => $description
    );
  }

  $form['actions'] = array(
    '#type' => 'actions'
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#disabled' => !empty($errors),
    '#value' => t('Delete template'),
    '#attributes' => array('class' => array('button-danger')),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/structure/paragraphs/layouts',
  );

  $form['#attributes']['class'][] = 'confirmation';
  $form['#theme'] = 'confirm_form';

  return $form;
}

/**
 * Submit handler that deletes a flexible Layout Paragraphs template.
 *
 * @see layout_paragraphs_template_delete_form()
 */
function layout_paragraphs_template_delete_form_submit(&$form, &$form_state) {
  /* @var LayoutFlexibleTemplate $layout_paragraphs_template */
  $layout_paragraphs_template = &$form_state['flexible_template'];
  $layout_paragraphs_template->delete();

  backdrop_set_message(t('The layout template "@title" has been deleted.', array('@title' => $layout_paragraphs_template->title)));
  $form_state['redirect'] = 'admin/structure/paragraphs/layouts';
}

/**
 * Generate an SVG image to be used as a Paragraphs layout template thumbnail.
 * @todo If this goes in core it may be possible to use that and remove this.
 * @see https://github.com/backdrop/backdrop-issues/issues/4104
 *
 * @param $template
 *   A layout template object.
 *
 * @return string $svg
 *   The SVG of the generated thumbnail image.
 *
 *   This function only works with the GD toolkit. ImageMagick is not supported.
 */
function _layout_paragraphs_template_generate_svg($template) {
  $template_rows = $template->rows;
  $flexi_styles = layout_flexible_row_styles();
  $thumbnail_rows = array();
  $row_color = 'currentColor';
  $svg = "<div class='layout-icon'><svg width='100%' height='100%' viewBox='0 0 100 100' preserveAspectRatio='none'>";
  $thumb_width = 100;
  $thumb_height = 100;
  $separator_size = 3;
  foreach ($template_rows as $row) {
    $thumbnail_rows[] = $flexi_styles[$row['contains']]['split'];
  }
  $rows_count = count($thumbnail_rows);
  $horizontal_separator_count = $rows_count - 1;
  $available_vertical_space = $thumb_height - ($horizontal_separator_count * $separator_size);
  // Divide the vertical space among the number of rows.
  $row_height = ($rows_count) ? $available_vertical_space / ($rows_count) : $available_vertical_space;

  $starting_y = 0;
  foreach ($thumbnail_rows as $index => $region_split) {
    $splits = explode(':', $region_split);
    $regions_count = count(array_filter($splits));
    $vertical_separator_count = $regions_count - 1;
    $available_horizontal_space = $thumb_width - ($vertical_separator_count * $separator_size);
    $current_row_height = $row_height;
    $starting_x = 0;
    foreach ($splits as $split) {
      if (!empty($split) && $starting_x < $thumb_width) {
        $split_width = ($split * $available_horizontal_space) / 100;
        $svg .= "<rect x='$starting_x%' y='$starting_y%' width='" . $split_width . "%' height='" . $current_row_height . "%' fill='$row_color' />";
        $starting_x += $split_width + $separator_size;
      }
    }

    $starting_y += $current_row_height + $separator_size;
  }

  $svg .= "</svg></div>";
  return $svg;
}
