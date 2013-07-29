This is Karl's holding area for contiki work.

I _hope_ to get most/all of this upstreamed some day, but I need my own
place for some of this in the meantime.

Branch policy
=============
This is a feature/dev fork, lots of branches will be getting rebased, and
force pushed!

@master _will_ get rebased!  You have been warned!  I generally try not to,
but changes like this README.md file need to be somewhere :)  I try to keep
@master rebased on top of upstream/master as much as possible, but I can be
behind.

@locm3 will get rebased, probably quite often!  It contains all the common
cpu code for (at present) stm32's based on libopencm3.  This is the main focus
of this repository

Other branches are my own dev work, or for show-n-tell, and may come or go
without much notice :)

You may also want to see https://github.com/karlp/contiki-outoftree which
_uses_ this repository.

The Contiki Operating System
============================

[![Build Status](https://secure.travis-ci.org/contiki-os/contiki.png)](http://travis-ci.org/contiki-os/contiki)

Contiki is an open source operating system that runs on tiny low-power
microcontrollers and makes it possible to develop applications that
make efficient use of the hardware while providing standardized
low-power wireless communication for a range of hardware platforms.

Contiki is used in numerous commercial and non-commercial systems,
such as city sound monitoring, street lights, networked electrical
power meters, industrial monitoring, radiation monitoring,
construction site monitoring, alarm systems, remote house monitoring,
and so on.

For more information, see the Contiki website:

[http://contiki-os.org](http://contiki-os.org)
