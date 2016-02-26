#Projection Mapping Workshop

Instructor: David Rueter  
Visiting Assistant Professor, New Media Arts, UIC  
netid: darueter

## Projectors

Tour of projectors, how to use them.

* Connectors - HDMI, DVI, VGA
* Brightness - Lumens
* Contrast
* Resolution
* Technologies - LCD, DLP, LCoS
* warm-up / cool-down

## Projection Mapping

### Overview of Topics
* Masking
* Skewing / perspective transform
* Content sources (Movies, Syphon, livecoding of shaders)
* Multiple videos one screen 
* Using multiple projectors for a single video

### To Consider

Questions to keep in mind: What does this technology / technique seem to ask for? Part of its appeal seems to be its novelty, or wow-factor, but in 2016, this won't get you very far. How is it typically used? How might those typical uses be questioned, critiqued, or developed further? What kinds of relationships between artist, architecture, public space, and audience are presupposed, generated, or transformed by this approach?

### References

For example:
http://www.creativebloq.com/video/projection-mapping-912849

Amon Tobin:  
https://vimeo.com/61257146

Prague Astronomical Clock:  
https://www.youtube.com/watch?v=in9BgR-P5b4

https://www.youtube.com/watch?v=atJcAT2Y294

Laszlo Maholy-Nagy, light play:  
https://vimeo.com/111650264  
http://www.medienkunstnetz.de/works/lichtspiel/

Jan Tichy:  
https://vimeo.com/61805899  
https://vimeo.com/117874453  
https://www.youtube.com/watch?v=yOrwJwbFfYo

Krzysztof Wodiczko  
https://www.youtube.com/watch?v=juq-Z48lY7g&list=PL210428AFC3F67FEE&index=10

Jenny Holzer projection - Chicago
https://www.youtube.com/watch?v=QeOOFKzQxS0

AM-CB  
https://www.youtube.com/watch?v=qAYAuJ9dIlE

Dev Harlan
http://www.devharlan.com/doku.php


### Software overview:
* [MadMapper](http://www.madmapper.com/) (Suports Syphon, popular, easy to use, non-free)
* [VPT](https://hcgilje.wordpress.com/vpt/) (Free, cross-platform, don't try to use HD videos, it'll grind to a halt)
* [Kibio](https://github.com/kibio/kibio/releases) (Free, cross-platform, developed at SAIC, video files only)
* [ofxPiMapper](https://github.com/kr15h/ofxPiMapper) (Free, works on Raspberry Pi)
* [VMAP](https://github.com/AlanChatham/VMap/releases) (Free, cross-platform, Processing library. I made a [sample sketch that works with Syphon]().)
* [ofxMtlMapping2D](https://github.com/morethanlogic/ofxMtlMapping2D) (Free, cross-platform, library for OpenFrameworks)
* [Virtual Mapper](https://github.com/baku89/VirtualMapper) (open source, cross platform, Syphon + Spout, annoying text in the corner you can't remove!!! If you're comfortable with openFrameworks you can recompile and fix this)
* [Painting With Light](http://www.bigfug.com/software/painting-with-light/#downloads)
* [Splash](https://github.com/paperManu/splash/wiki) (Free, geared towards mapping of domes)
* [LPMT](http://hv-a.com/lpmt/) (Not free but cheap, cross-platform, works with MostPixelsEver)
* [HeavyM](https://heavym.net/en/heavym-software) (Beta version, cross-platform)
* [Dynamapper](http://dynamapper.net/) (iOS, Android, free low-res version)
* [MapMap](http://mapmap.info/tiki-index.php)
* [Mapamok](https://github.com/YCAMInterlab/ProCamToolkit/wiki/mapamok-(English\)) (Free, cross-platform, doesn't accept video input, designed for experiments with graphics shaders and complex room/object geometries.)




#### VideoProjectionTool (VPT):
https://hcgilje.wordpress.com/vpt/

Tip: Mavericks (and later note: to get fullscreen working, you have to go to:  
system preferences- mission control, then disable "displays have separate spaces"

Tutorial: https://www.youtube.com/watch?feature=player_embedded&v=6tPGP0Ga8a8

Size your sketch to match the aspect ratio of the surface you're projecting onto.


### Syphon

Clients / Server approach to sharing video streams on OSX. (Local machine only, not networkable)

http://syphon.v002.info/

[Syphoner](http://www.sigmasix.ch/syphoner/) - an app that streams any OSX app window to Syphon

After effects comps live in MadMapper/VPT
[AESyphon](http://tobiasebsen.dk/experiments/aesyphon/)

Syphon library for Processing - install via Import Library -> Add Library

OpenFrameworks: [ofxSyphon](https://github.com/astellato/ofxSyphon)

#### Windows version - Spout

### Other tools
https://github.com/shiffman/Most-Pixels-Ever-Processing - Most Pixels Ever



