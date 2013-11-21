/* Copyright (c) 2009 Mustafa OZCAN (http://www.mustafaozcan.net)
 * Dual licensed under the MIT (http://www.opensource.org/licenses/mit-license.php)
 * and GPL (http://www.opensource.org/licenses/gpl-license.php) licenses.
 * Version: 1.0.2
 * Requires: jquery.1.3+
 * Heavily enhanced by Peter Waller <peter.waller@cern.ch> (Oct 2010)
 */
jQuery.fn.fixedtableheader = function (options) {
    var settings = jQuery.extend({
        first_column: false,
        fixed_class: "fixedtableheader"
    }, options);
    
    function fix_table_position(from, to, fix_left) {
        if (jQuery.browser.msie) 
            to.css({
                "position": "absolute",
                "top" : fix_left ? from.offset().top      : $(window).scrollTop(),
                "left": fix_left ? $(window).scrollLeft() : from.offset().left
            });
        else 
            to.css({
                "position": "fixed",
                "top" : fix_left ? from.offset().top - $(window).scrollTop() : "0",
                "left": fix_left ? "0" : from.offset().left - $(window).scrollLeft()
            });
    }
    
    function fix_element_sizes(from, to, force) {
        var force = typeof(force) != 'undefined' ? force : false;
        
        var header_elements = settings.first_column ? from.find("tr") : from.find("th,td");
        var new_header_elements = settings.first_column ? to.find("tr") : to.find("th,td");
        
        // Check if the table changed size
        if (settings.first_column && (to.outerHeight() != from.outerHeight() || force)) {        
            new_header_elements.each(function (index) {
                $(this).css("height", header_elements.eq(index).height())
            });
            to.height(from.outerHeight());
            
        } else if (!settings.first_column && (to.outerWidth() != from.outerWidth() || force)) {
            new_header_elements.each(function (index) {
                $(this).css("width", header_elements.eq(index).width())
            });
            to.width (from.outerWidth() );
        }
        if (settings.first_column)
            to.css("top",  from.offset().top );
        else
            to.css("left", from.offset().left);            
    }
    
    function clone_column(header_elements) {
        // Need a special clone function to pick out a <tr> with just the first <td>
        var new_thead = $("<thead />");
        var new_tbody = $("<tbody />");
        
        colspans = header_elements.map(function() { return $(this).attr("colspan"); });
        widest_colspan = Math.max.apply(Math, colspans.toArray())
        
        header_elements.each(function () {
            self = $(this).clone();
            self_in_tr = $(this).parent().clone().empty().append(self);
            if (self.attr("colspan") < widest_colspan)
                self_in_tr.append($("<td />").attr("colspan", widest_colspan - self.attr("colspan")));
            if (self[0].nodeName == "TH")
                new_thead.append(self_in_tr);
            else
                new_tbody.append(self_in_tr);
        });
        return new_thead.before(new_tbody);
    }
    
    var original_table = $(this).parents("table");
    if (original_table.length != 1)
        throw new Error("Invalid selector for fixedtableheader - should be heading")
    
    var original_header_elements = $(this);
    
    var header_elements = original_header_elements.find("th,td");
    var new_header_elements = settings.first_column 
        ? clone_column(original_header_elements) 
        : original_header_elements.clone();
    
    // No header elements. 
    if (!header_elements) return;
    
    var fixed_header_table = original_table.clone().empty()
        .append(new_header_elements)
        .addClass(settings.fixed_class)
        .hide(false)
        .appendTo($("body"));
    
    fix_table_position(original_header_elements, fixed_header_table, settings.first_column);
    fix_element_sizes (original_header_elements, fixed_header_table, true);
    
    function determine_hidden() {
        var scroll_pos = settings.first_column ? $(window).scrollLeft() : $(window).scrollTop();
        var elm_pos = settings.first_column ? original_header_elements.offset().left : original_header_elements.offset().top;
        var elm_size = settings.first_column
            ? original_table.width()  - original_header_elements.find("td:not([colspan]").width()
            : original_table.height() - original_header_elements.height();
        
        if (scroll_pos > elm_pos && scroll_pos <= (elm_pos + elm_size))
            fixed_header_table.show();
        else 
            fixed_header_table.hide();
    }

    $(window).scroll(function () {
        fix_table_position(original_table, fixed_header_table, settings.first_column);
        determine_hidden();
    });
    
    $(window).resize(function () {
        fix_element_sizes(original_table, fixed_header_table);
    });
};
