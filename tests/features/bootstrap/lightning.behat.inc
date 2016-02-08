<?php

/**
 * @file
 * Contains \LightningSubContext.
 */

use Behat\Behat\Hook\Scope\BeforeScenarioScope;
use Drupal\DrupalExtension\Context\DrupalSubContextBase;
use Drupal\DrupalExtension\Context\DrupalSubContextInterface;

/**
 * Sub context for Lightning step definitions.
 */
class LightningSubContext extends DrupalSubContextBase implements DrupalSubContextInterface {

  /**
   * The Drupal context.
   *
   * @var \Drupal\DrupalExtension\Context\DrupalContext
   */
  protected $drupalContext;

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
  public function gatherContexts(BeforeScenarioScope $scope) {
    $environment = $scope->getEnvironment();

    $this->drupalContext = $environment->getContext('Drupal\DrupalExtension\Context\DrupalContext');
    $this->minkContext = $environment->getContext('Drupal\DrupalExtension\Context\MinkContext');
  }


  /**
   * Asserts that a CKEditor instance exists.
   *
   * @param string $id
   *   The editor's instance ID in CKEDITOR.instances.
   *
   * @throws \Exception
   *   If the specified CKEditor instance does not exist.
   *
   * @Given CKEditor :id exists
   *
   * @Then CKEditor :id should exist
   */
  public function ckEditorShouldExist($id) {
    $exists = $this->getSession()
      ->evaluateScript("CKEDITOR.instances.hasOwnProperty('$id');");

    if ($exists == FALSE) {
      throw new \Exception("CKEditor '$id' does not exist.");
    }
  }

  /**
   * Puts text or HTML into a CKEditor instance.
   *
   * @param string $text
   *   The text (or HTML) to insert into the editor.
   * @param string $id
   *   The editor's instance ID in CKEDITOR.instances.
   *
   * @When I put :text into CKEditor :id
   */
  public function iPutTextIntoCkEditor($text, $id) {
    $this->getSession()
      ->executeScript("CKEDITOR.instances['$id'].insertHtml('$text');");
  }

  /**
   * Asserts that a CKEditor's content contains a snippet of text.
   *
   * @param string $id
   *   The editor's instance ID in CKEDITOR.instances.
   * @param string $text
   *   The text (or HTML) snippet to look for.
   *
   * @throws \Exception
   *   If the editor doesn't contain the specified text.
   *
   * @Then CKEditor :id should contain :text
   */
  public function ckEditorShouldContain($id, $text) {
    $html = $this->getCkEditorContent($id);

    if (strpos($html, $text) == FALSE) {
      throw new \Exception("CKEditor $id did not contain '$text''.");
    }
  }

  /**
   * Assert that a CKEditor's content matches a regular expression.
   *
   * @param string $id
   *   The editor's instance ID in CKEDITOR.instances.
   * @param string $expression
   *   The regular expression to match.
   *
   * @throws \Exception
   *   If the expression does not match.
   *
   * @Then CKEditor :id should match :expression
   */
  public function ckEditorShouldMatch($id, $expression) {
    $html = $this->getCkEditorContent($id);

    if (preg_match($expression, $html) == 0) {
      throw new \Exception("CKEditor $id did not match '$expression'.");
    }
  }

  /**
   * Gets the content of a CKEditor instance.
   *
   * @param string $id
   *   The editor instance ID in CKEDITOR.instances.
   *
   * @return string
   *   The HTML content of the editor.
   */
  protected function getCkEditorContent($id) {
    return $this->getSession()
      ->evaluateScript("CKEDITOR.instances['$id'].getData();");
  }

  /**
   * Executes a CKEditor command.
   *
   * @param string $command
   *   The command ID, as known to CKEditor's API.
   * @param string $id
   *   The editor's instance ID in CKEDITOR.instances.
   *
   * @When I execute the :command command in CKEditor :id
   */
  public function iExecuteTheCommandInCkeditor($command, $id) {
    $this->getSession()
      ->evaluateScript("CKEDITOR.instances['$id'].execCommand('$command');");
  }

  /**
   * Sleeps for a certain number of seconds.
   *
   * @When wait :seconds seconds
   * @When wait :second second
   * @When I wait :seconds seconds
   * @When I wait :seconds second
   */
  public function waitSeconds($seconds) {
    $this->getSession()->wait(1000 * $seconds);
  }

  /**
   * Asserts that an element, specified by CSS selector, exists.
   *
   * @Then the element :selector should exist
   */
  public function theElementShouldExist($selector) {
    $this->assertSession()->elementExists('css', $selector);
  }

}