# Foto - photo rename tool

Using libexif-0.6.21 from http://libexif.sourceforge.net/

## Clean project
~~~bash
$ make clean
~~~

## Build project
~~~bash
$ make
~~~

## Program usage
~~~bash
$ ./foto 
Usage: ./foto "path to your photo directory" <options>
 Options available: 						Example:
 -e<file extension to be included in rename operation>		-ejpg -eavi -emp4 -ethm
 -m<use default data mtime & size if picture data fails>	-mjpg
~~~

### Example of usage
~~~bash
$ ./foto "/home/user/FolderWithPicturesAndMovies" -ejpg -emp4
~~~

### Example with fallback to default file information if required
~~~bash
$ ./foto "/home/user/FolderWithPictures" -ejpg -mjpg
~~~

### Example results after rename operation
~~~bash
2006-03-21 13.04.30 Sony Ericsson W800i.jpg
2017-07-21 14.04.57 Sony G8231.jpg
2001-04-29 16.16.17 SONY CYBERSHOT.jpg
2003-12-02 19.57.38 SONY DSC-P92.jpg
2006-07-04 14.01.12 Canon Canon DIGITAL IXUS 50.jpg
2017-06-27 07.29.14(56MB).mp4
~~~