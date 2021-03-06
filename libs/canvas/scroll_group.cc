/*
    Copyright (C) 2014 Paul Davis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <iostream>

#include "pbd/compose.h"

#include "canvas/canvas.h"
#include "canvas/debug.h"
#include "canvas/scroll_group.h"

using namespace std;
using namespace ArdourCanvas;

ScrollGroup::ScrollGroup (Canvas* c, ScrollSensitivity s)
	: Container (c)
	, _scroll_sensitivity (s)	
{
}

ScrollGroup::ScrollGroup (Item* parent, ScrollSensitivity s)
	: Container (parent)
	, _scroll_sensitivity (s)	
{
}

void
ScrollGroup::render (Rect const & area, Cairo::RefPtr<Cairo::Context> context) const
{
	/* clip the draw to the area that this scroll group nominally occupies
	 * WITHOUT scroll offsets in effect
	 */

	boost::optional<Rect> r = bounding_box();

	if (!r) {
		return;
	}

	Rect self (_position.x, _position.y, _position.x + r.get().width(), _position.y + r.get().height());

	self.x1 = min (_position.x + _canvas->width(), self.x1);
	self.y1 = min (_position.y + _canvas->height(), self.y1);

	context->save ();
	context->rectangle (self.x0, self.y0, self.width(), self.height());
	context->clip ();
	
	Container::render (area, context);

	context->restore ();
}

void
ScrollGroup::scroll_to (Duple const& d)
{
	if (_scroll_sensitivity & ScrollsHorizontally) {
		_scroll_offset.x = d.x;
	}

	if (_scroll_sensitivity & ScrollsVertically) {
		_scroll_offset.y = d.y;
	}
}

bool
ScrollGroup::covers_canvas (Duple const& d) const
{
	boost::optional<Rect> r = bounding_box ();

	if (!r) {
		return false;
	}

	return r->contains (d);
}

bool
ScrollGroup::covers_window (Duple const& d) const
{
	boost::optional<Rect> r = bounding_box ();

	if (!r) {
		return false;
	}

	Rect w = r->translate (-_scroll_offset);

	return w.contains (d);
}
