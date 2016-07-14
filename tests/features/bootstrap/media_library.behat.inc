<?php

/**
 * @file
 * Contains \MediaLibrarySubContext.
 */

use Behat\Mink\Exception\ExpectationException;
use Drupal\DrupalExtension\Context\DrupalSubContextBase;
use Drupal\DrupalExtension\Context\DrupalSubContextInterface;

/**
 * Contains step definitions for testing the CKEditor media widget.
 */
class MediaLibrarySubContext extends DrupalSubContextBase implements DrupalSubContextInterface {

  /**
   * The Mink context.
   *
   * @var \Drupal\DrupalExtension\Context\MinkContext
   */
  protected $minkContext;

  /**
   * Pre-scenario hook.
   *
   * @BeforeScenario
   */
  public function gatherContexts() {
    $this->minkContext = $this->getContext('\Drupal\DrupalExtension\Context\MinkContext');
  }

  /**
   * Awaits the presence of an element on the page.
   *
   * @param string $selector
   *   The element's CSS selector.
   */
  protected function awaitElement($selector) {
    $this->await('document.querySelector("' . addslashes($selector) . '")');
  }

  /**
   * Waits 10 seconds for a JavaScript condition to be truthy.
   *
   * @param string $condition
   *   The JavaScript condition to wait for.
   *
   * @throws \Exception
   *   If the condition times out.
   */
  protected function await($condition) {
    $done = $this->getSession()->wait(10000, $condition);
    if ($done == FALSE) {
      throw new \Exception('JavaScript condition timed out: ' . $condition);
    }
  }

  /**
   * Waits for the inline entity form (containing required fields) to exist.
   */
  protected function awaitEntityForm() {
    $this->minkContext->iWaitForAjaxToFinish();
    $this->await('document.getElementById("ief-target").textContent.length');
  }

  /**
   * Uploads a file in the media browser.
   *
   * @param string $file
   *   The path to the file, relative to the test files directory.
   *
   * @When I upload :file
   */
  public function upload($file) {
    $this->minkContext->clickLink('Upload');
    $this->minkContext->attachFileToField('File', $file);
    $this->awaitEntityForm();
  }

  /**
   * Enters an embed code in the media browser.
   *
   * @param string $code
   *   The embed code.
   *
   * @When I enter embed code :code
   */
  public function embed($code) {
    $this->minkContext->clickLink('Create Embed');
    $this->minkContext->fillField('embed_code', $code);
    // The change event, which triggers AJAX, is fired automatically after 600
    // milliseconds.
    sleep(1);
    $this->awaitEntityForm();
  }

  /**
   * Opens up an image browser.
   *
   * @param string $label
   *   The label of the field containing the image browser.
   *
   * @When I open the image browser for :label
   */
  public function openImageBrowser($label) {
    /** @var \CollapsibleSubContext $context */
    $context = $this->getContext('\CollapsibleSubContext');

    $context->openCollapsible($label);
    $this->minkContext->pressButton('Select Image(s)');
    $this->minkContext->iWaitForAjaxToFinish();
    $this->getSession()->switchToIFrame('entity_browser_iframe_image-browser');
    $this->awaitElement('form.entity-browser-form');
  }

  /**
   * Selects an item in the entity browser by its position.
   *
   * @param int $n
   *   The item's one-based position in the grid view.
   *
   * @When I select item :n in the browser
   */
  public function selectBrowserItem($n) {
    // Determine where in the grid the item will be and build the selector.
    $row = floor($n / 4) + 1;
    $column = ($n % $row) + 1;
    $selector = ".view.eb-media .row-{$row} .col-{$column}[data-selectable]";

    /** @var \LightningSubContext $context */
    $context = $this->getContext('\LightningSubContext');
    $context->clickElementBySelector($selector);

    $this->completeBrowserSelection();
  }

  /**
   * Completes the process of selecting an image in the image browser.
   *
   * @When I complete the image browser selection
   */
  public function completeBrowserSelection($button = 'Select') {
    $this->minkContext->pressButton($button);
    // $this->getSession()->switchToWindow();
    $this->minkContext->iWaitForAjaxToFinish();
  }

  /**
   * Asserts that at least one entity is selected for an entity browser field.
   *
   * @param string $field_label
   *   The label of the field that uses the entity browser.
   *
   * @throws \Behat\Mink\Exception\ExpectationException
   *   If no entities are selected.
   *
   * @Then an entity should be selected for :field_label
   */
  public function assertEntitySelected($field_label) {
    /** @var \CollapsibleSubContext $context */
    $context = $this->getContext('\CollapsibleSubContext');

    $is_empty = $context->assertCollapsible($field_label)
      ->find('css', 'table.entities-list tbody tr td.empty.message');

    if ($is_empty) {
      throw new ExpectationException('Expected at least one entity to be selected for field "' . $field_label . '".', $this->getSession()->getDriver());
    }
  }

}
