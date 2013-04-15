=========
 websnap
=========

:License: `MIT <http://opensource.org/licenses/MIT>`__
:Author: Adrian Perez de Castro <aperez@igalia.com>

This is a small tool, loosely based upon `CutyCapt
<http://cutycapt.sourceforge.net/>`__, which repeatedly renders
web pages using QtWebKit on a ``QImage``. It is suitable to measure
rendering speed of the Qt software raster engine. This is particularly
interesting for people working on optimizing it (or QtWebKit). There is
support for controlling the `Google Performance Tools
<https://gperftools.googlecode.com/>`__ `CPU Profiler
<https://gperftools.googlecode.com/svn/trunk/doc/cpuprofile.html>`__
and getting meaningfull data only from the rendering process (skipping
e.g. network access).


Building
========

The following must be installed and useable:

* Qt 4.8 with QtWebKit. QtEmbedded should work, but you might need to
  have it built with a `patch for QWSLock`__.
* GNU Make.

__ http://www.mail-archive.com/community@lists.openmoko.org/msg65512.html

For building, follow the usual procedure for Qt-based applications::

  qmake && make


Running
=======

Standalone::

  ./websnap URL num-renders - output.png

With the Google Performance Tools CPU profiler::

  LD_PRELOAD=/usr/lib/libprofiler.so \
  ./websnap URL num-renders output.profile output.png

All arguments are optional:

* The default ``URL`` is http://igalia.com.
* The default ``num-renders`` is ``1``.
* The profiler and PNG outputs are empty (disabled) by default.

