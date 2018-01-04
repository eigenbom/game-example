/**
 * An implementation of an API similar to ncurses, but which instead runs on
 * HTMl5 canvas.
 *
 * Specifically, this is designed to duplicate the following elements of ncurses:
 *
 *  - Basic text printing
 *  - Colors
 *  - Handling window hierarchies, which respect their parents bounds (so that
 *    drawing doesn't occur outside of a window's boundaries). They have their own
 *    cursor positions, but do not have their own foreground and background colors,
 *    nor do they have their own character arrays (all this is shared with their parent
 *    window, and ultimately the root window).
 * 
 * To use this, do something like the following:
 * 
 *   var root = RootWindow(<some HTML element>, cols, rows);
 *   root.write_str('Hello, World!');
 *   root.refresh();
 * 
 * The RootWindow will attempt to find the best font size, although you may override
 * it as follows:
 * 
 *   var root = RootWindow(<some HTML element>, cols, rows, font_family, font_size_in_px);
 * 
 * The rest of the methods are documented below. Note that the methods implemented
 * for subwindows (under SubwindowFuncs) are intended to be the same as their counterparts
 * in the RootWindow.
 */

/**
 * Names for keys which are spit out by the keyCode field on most key events.
 */
var CursesKey = {
    'Backspace': 8,
    'Tab': 9,
    'Enter': 13,
    'Shift': 16,
    'Control': 17,
    'Alt': 18,
    'CapsLock': 20,
    'Escape': 27,
    'LeftMeta': 91,
    'RightMeta': 92,
    'Menu': 93,

    'PageUp': 33,
    'PageDown': 34,
    'Home': 36,
    'End': 35,

    'LeftArrow': 37,
    'RightArrow': 39,
    'UpArrow': 38,
    'DownArrow': 40,

    'Insert': 45,
    'Delete': 46,

    'Zero': 48,
    'One': 49,
    'Two': 50,
    'Three': 51,
    'Four': 52,
    'Five': 53,
    'Six': 54,
    'Seven': 55,
    'Eight': 56,
    'Nine': 57,

    'A': 65,
    'B': 66,
    'C': 67,
    'D': 68,
    'E': 69,
    'F': 70,
    'G': 71,
    'H': 72,
    'I': 73,
    'J': 74,
    'K': 75,
    'L': 76,
    'M': 77,
    'N': 78,
    'O': 79,
    'P': 80,
    'Q': 81,
    'R': 82,
    'S': 83,
    'T': 84,
    'U': 85,
    'V': 86,
    'W': 87,
    'X': 88,
    'Y': 89,
    'Z': 90,

    'SemiColon': 186,
    'Equal': 187,
    'Comma': 188,
    'Hyphen': 189,
    'Period': 190,
    'Slash': 191,
    'Grave': 192,
    'LeftBracket': 219,
    'Backslash': 220,
    'RightBracket': 221,
    'Quote': 222
};

var RootWindowFuncs = {
    /**
     * Tests to see if a coordinate is inside this root window.
     */
    'in_window': (function(x, y) {
        return (x >= 0 && x < this.width &&
                y >= 0 && y < this.height);
    }),
    
    /**
     * Computes the index into the character/color array given an (x, y) 
     * coordinate.
     */
    'index': (function(x, y) {
        return x + (this.width * y);
    }),

    /**
     * Sets the current foreground color.
     */
    'set_fg': (function(color) {
        this.current_foreground = this.get_color(color);
    }),

    /**
     * Sets the current background color.
     */
    'set_bg': (function(color) {
        this.current_background = this.get_color(color);
    }),

    /**
     * Defines an alias for a color; note that aliases can be rebound (that is,
     * binding the same alias twice is not an error, it just overwrites the old
     * one).
     */
    'define_color': (function(name, color) {
        this.defined_colors[name] = color;
    }),

    /** 
     * Resolves an alias to a color, returning either the color pointed to by
     * the alias, or the original input.
     */
    'get_color': (function(name) {
        if (name in this.defined_colors)
            return this.defined_colors[name];
        else
            return name;
    }),

    /**
     * Moves the cursor to the given (x, y) location. Note that if either of
     * them are outside the bounds of the canvas, then this sets the (x, y) to
     * (0, 0).
     */
    'move': (function(x, y) {
        if (!this.in_window(x, y)) {
            this.x = 0;
            this.y = 0;
        } else {
            this.x = x;
            this.y = y;
        }
    }),
    
    /**
     * Moves down to the beginning of the next line, going to the coordinate (0, 0)
     * if the current line is the last line.
     */
     'nextline': (function() {
        this.x = 0;
        this.y++;
        if (this.y >= this.height)
            this.y = 0;
     }),

    /**
     * Writes a character, advancing the cursor. If the cursor is at the end
     * of the current line, the line is advanced as described in the
     * nextline() function.
     */
    'write_ch': (function(char) {
        this.set_ch(char, this.x, this.y);

        this.x++;
        // If we've hit the end of the row, then advance to the next
        if (this.x >= this.width)
            this.nextline();
    }),

    /**
     * Writes out an entire string, using write_ch().
     */
    'write_str': (function(str) {
        for (var i = 0; i < str.length; i++)
            this.write_ch(str[i]);
    }),

    /**
     * Gets the character at the given location, or if the coordinate is
     * invalid, this returns the empty string instead.
     */
    'get_ch': (function(cx, cy) {
        if (!this.in_window())
            return '';

        var index = this.index(cx, cy);
        return this.char_memory[index];
    }),

    /**
     * Assigns the character at the given location, optionally using the given
     * foreground and background color (otherwise, they default to the current 
     * colors).
     */
    'set_ch': (function(char, cx, cy, bg_color, fg_color) {
        if (!this.in_window(cx, cy))
            return;
            
        bg_color = bg_color || this.current_background;
        fg_color = fg_color || this.current_foreground;
        var index = this.index(cx, cy);

        this.char_memory[index] = char;
        this.color_memory[index].foreground = fg_color;
        this.color_memory[index].background = bg_color;
    }),

    /**
     * Creates a new subwindow, which covers the given rectangle. Note that
     * the subwindow shares style information with this parent, and uses the
     * same character buffers to do drawing.
     * 
     * Unlike ncurses, however, subwindows do bounds checking, and thus you
     * cannot write outside the bounds of the subwindow, and end up polluting
     * areas of the parent window not covered by the subwindow.
     */
    'subwindow': (function(x, y, width, height) {
        return Subwindow(this, x, y, width, height);
    }),

    /**
     * Draws a border around the edge of this window, using the given characters
     * for the left, right, top and bottom lines.
     */
    'border': (function(leftchar, rightchar, topchar, bottomchar) {
        for (var row = 0; row < this.height; row++) {
            this.set_ch(leftchar, 0, row);
            this.set_ch(rightchar, this.width - 1, row);
        }

        for (var col = 0; col < this.width; col++) {
            this.set_ch(topchar, col, 0);
            this.set_ch(bottomchar, col, this.height - 1);
        }
    }),

    /**
     * Gets the 2D context for this window.
     */
    'get_context': (function() {
        return this.canvas.getContext('2d');
    }),

    /**
     * Clears a cell, setting it to the current foreground and background while
     * erasing whatever character was there.
     */
    'clear_cell': (function(cx, cy) {
        if (!this.in_window(cx, cy))
            return;

        var index = this.index(cx, cy);
        this.char_memory[index] = ' ';
        this.color_memory[index].foreground = this.current_foreground;
        this.color_memory[index].background = this.current_background;
    }),

    /**
     * Clears the screen, removing all text and setting the colors to the current
     * colors. Note that this does not cause the screen to be redrawn, however.
     */
    'clear': (function() {
        for (var row = 0; row < this.height; row++) {
            for (var col = 0; col < this.width; col++)
                RootWindowFuncs.clear_cell.apply(this, [col, row]);
        }
    }),

    /**
     * Renders a single cell to the screen.
     */
    'refresh_cell': (function(context, cx, cy) {
        if (!this.in_window(cx, cy))
            return;

        var index = this.index(cx, cy);
        var left_x = cx * this.aspect_x;
        var right_x = (cx + 1) * this.aspect_x;
        var top_y = cy * this.aspect_y;
        var bottom_y = (cy + 1) * this.aspect_y;
        var text_offset = - (bottom_y - top_y) / 4;

        context.fillStyle = this.color_memory[index].background;
        context.fillRect(left_x, top_y, right_x - left_x, bottom_y - top_y);

        context.fillStyle = this.color_memory[index].foreground;
        context.fillText(this.char_memory[index], (left_x + right_x) / 2, bottom_y + text_offset);
    }),

    /**
     * Draws the screen. 
     * 
     * Note that this uses the simplest method possible - that is, it draws all cells,
     * regardless of whether they have been modified since the last call. ncurses
     * is a bit smarter, and I'll have to see if there is enough of a performance
     * difference to justify the additional complexity of adding bookeeping to draw
     * only the differences.
     */
    'refresh': (function() {
        var context = this.get_context();
        context.font = this.font_size + 'px ' + this.font;
        context.textAlign = 'center';

        // Note that we go from the bottom to the top, since there are characters
        // like the comma which will stick below the cell, and get chopped off by
        // the bottom cell. Going from the bottom, we don't chop off those
        // bottom pieces, since they get drawn over the bottom cell, which has already
        // been rendered.
        for (var row = this.height - 1; row >= 0; row--) {
            for (var col = 0; col < this.width; col++)
                RootWindowFuncs.refresh_cell.apply(this, [context, col, row]);
        }
    }),
};

function RootWindow(canvas, width, height, font, font_size) {
    var self = Object.create(RootWindowFuncs);
    self.canvas = canvas;
    self.width = width;
    self.height = height;
    self.x = 0;
    self.y = 0;

    self.aspect_x = canvas.width / width;
    self.aspect_y = canvas.height / height;

    // The height of the font we're choosing, courier, should be equal to the
    // height of the font size
    self.font = font || 'courier';
    self.font_size = font_size || Math.floor(canvas.height / (height * 1.1));

    self.current_foreground = 'rgb(255,255,255)';
    self.current_background = 'rgb(0,0,0)';

    // This holds all the defined colors, for example, {'black': 'rgb(0,0,0)'}
    self.defined_colors = {};

    // This stores the characters for each cell on the screen; the index of
    // a given character is char_memory[(width * y) + x];
    self.char_memory = [];
    for (var row = 0; row < height; row++) {
        for (var col = 0; col < width; col++)
            self.char_memory.push(' ');
    }

    // This stores the foreground and background for each cell on the screen,
    // where each element contains an object which looks like this:
    // {'foreground': COLOR, 'background': COLOR}, where each COLOR is
    // represented as any color acceptable by CSS.
    self.color_memory = [];
    for (var row = 0; row < height; row++) {
        for (var col = 0; col < width; col++)
            self.color_memory.push({
                'foreground': 'rgb(255, 255, 255)',
                'background': 'rgb(0, 0, 0)'
            });
    }
    return self;
}

// Wraps a function provided by another object. This is used by
// subwindows to provide transparent access to the color functions
// of their parents.
function proxy(obj, name) {
    return (function() {
        return obj[name].apply(obj, arguments);
    });
}

var SubwindowFuncs = {
    'move': (function(x, y) {
        if (!this.in_window(x, y)) {
            this.x = 0;
            this.y = 0;
        } else {
            this.x = x;
            this.y = y;
        }
    }),
    
    'nextline': (function() {
        this.x = 0;
        this.y++;
        
        if (this.y >= this.height)
            this.y = 0;
    }),

    'write_ch': (function(char) {
        this.set_ch(char, this.x, this.y);

        this.x++;
        if (this.x >= this.width)
            this.nextline();
    }),

    'write_str': (function(str) {
        for (var i = 0; i < str.length; i++)
            this.write_ch(str[i]);
    }),

    'get_ch': (function(cx, cy) {
        if (!this.in_window(cx, cy))
            return '';

        return this.parent.get_ch(cx + self.x_offset,
            cy + self.y_offset);
    }),
    'set_ch': (function(char, cx, cy, bg_color, fg_color) {
        if (!this.in_window(cx, cy))
            return '';

        return this.parent.set_ch(char, cx + this.x_offset, cy + this.y_offset, 
            bg_color, fg_color);
    }),

    'subwindow': (function(x, y, width, height) {
        return this.parent.subwindow(this.x_offset + x, this.y_offset + y, width, height);
    }),

    'border': (function(leftchar, rightchar, topchar, bottomchar) {
        for (var row = 0; row < this.height; row++)
        {
            this.set_ch(leftchar, 0, row);
            this.set_ch(rightchar, this.width - 1, row);
        }

        for (var col = 0; col < this.width; col++)
        {
            this.set_ch(topchar, col, 0);
            this.set_ch(bottomchar, col, this.height - 1);
        }
    }),

    'clear_cell': (function(cx, cy) {
        if (!this.in_window(cx, cy))
            return;

        this.parent.clear_cell(cx + this.x_offset, cy + this.y_offset);
    }),

    'clear': (function() {
        for (var row = 0; row < this.height; row++) {
            for (var col = 0; col < this.width; col++)
                this.clear_cell(col, row);
        }
    }),

    'refresh_cell': (function(context, cx, cy) {
        if (!this.in_window(cx, cy))
            return;

        this.parent.refresh_cell(context, cx + this.x_offset, cy + this.y_offset);
    }),

    'refresh': (function() {
        var context = this.parent.get_context();
        context.font = this.parent.font_size + 'px ' + this.parent.font;

        // As we do for the parent, we do for the child - both of them draw from the bottom
        // up, to preserve the 'hanging' bits of characters like commas
        for (var row = this.height - 1; row >= 0; row--) {
            for (var col = 0; col < this.width; col++)
                this.refresh_cell(context, col, row);
        }
    }),
};

function Subwindow(parent_win, x, y, width, height)
{
    var self = Object.create(SubwindowFuncs);
    self.parent = parent_win;
    self.x_offset = x;
    self.y_offset = y;
    self.x = 0;
    self.y = 0;
    self.width = width;
    self.height = height;

    self.in_window = RootWindowFuncs.in_window;

    self.set_fg = proxy(parent_win, 'set_fg');
    self.set_bg = proxy(parent_win, 'set_bg');

    self.define_color = proxy(parent_win, 'define_color');
    self.get_color = proxy(parent_win, 'get_color');

    return self;
}
