(function ($, Drupal, Backbone) {
  "use strict";

  window.MediaLibrary = Backbone.View.extend({

    widget: null,

    events: {
      'dialogclose': 'onDialogClose'
    },

    attributes: {
      class: 'media-library'
    },

    getActiveWidget: function (element) {
      switch (true) {
        case element === this.library.el:
          return this.library;
        case element === this.upload.el:
          return this.upload;
        case element === this.embedCode.el:
          return this.embedCode;
        default:
          break;
      }
    },

    /**
     * Event triggered when a tab is chosen.
     */
    onTabActivate: function (event, ui) {
      this.widget = this.getActiveWidget(ui.newPanel.get(0));
    },

    /**
     * Event triggered when the jQuery UI tab set is created.
     */
    onTabCreate: function (event, ui) {
      this.widget = this.getActiveWidget(ui.panel.get(0));
    },

    /**
     * Event triggered when a jQuery UI dialog box is closed.
     */
    onDialogClose: function () {
      this.$el.tabs('option', 'active', 0);
    },

    initialize: function () {
      this.library = new EntityGrid({
        backend:
          new MediaLibraryBackend([], { baseUrl: Drupal.url('media-library') })
      });

      this.upload = new Uploader({
        url: Drupal.url('lightning/upload')
      });
      this.listenTo(this.upload.model, 'sync', function (model) {
        this.library.backend.unshift(model);
      });

      this.embedCode = new EmbedCode({
        url: Drupal.url('lightning/embed-code')
      });
      this.listenTo(this.embedCode.model, 'sync', function (model) {
        this.library.backend.unshift(model);
      });

      this.render();
    },

    randomId: function () {
      var text = '';
      for (var i = 0; i < 8; i++) {
        text += 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'.charAt(Math.floor(Math.random() * 52));
      }
      return text;
    },

    addTab: function (element, title) {
      element.id = this.randomId();
      this.el.appendChild(element);
      return $('<li><a href="#' + element.id + '">' + Drupal.t(title) + '</a></li>');
    },

    render: function () {
      var nav = document.createElement('ul');

      this.addTab(this.library.el, 'Library').appendTo(nav);
      this.addTab(this.upload.el, 'Upload').appendTo(nav);
      this.addTab(this.embedCode.el, 'Embed Code').appendTo(nav);

      this.$el.prepend(nav).tabs({
        activate: this.onTabActivate.bind(this),
        create: this.onTabCreate.bind(this),
        show: 400
      });
    }

  });

})(jQuery, Drupal, Backbone);