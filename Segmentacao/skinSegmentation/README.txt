* Project Name: skinSegmentation 
* Author: Hemerson Pistori (pistori@ucdb.br)
* Institutions: Dom Bosco Catholic University and University of Bristol
* Last Update: September, 2012
* General Descrition:
   Segment an image using threshold defined by the user or calculated from sample. Drag mouse over the image to select a sample. 

* Folders:
  - bin: where the executables are put
  - src: project sources
  - data: auxiliary data 

* Some important files:
  - src/skinSegmentation.cpp: main source files
  - data/config.xml: configuration file

* How to compile (Linux Shell):
  $ rm CMakeCache.txt 
  $ cmake . 
  $ make

* How to run (Linux Shell):
  $ cd bin
  $ ./skinSegmentation

* How to use (Interface):

- Drag a rectangular region in the image captured from the webcam to select a region that will be used as a reference for the segmentation (binarization) module
- The sliders from Hmin to Range Size are related to the segmentation algorithm and can be used to improve it (read the source code to understand their meaning)

* Non Standard Libraries Dependencies:
  - opencv 2.4 or higher

* Installing Opencv (tested on kubuntu 12.04)
  $ open a terminal (E.g: konsole)
  $ sudo apt-get install libqt4-* libphonon-dev build-essential cmake pkg-config  libpng12-0 libpng12-dev libpng++-dev libpng3  libpnglite-dev   zlib1g-dbg zlib1g zlib1g-dev  libjasper-dev libjasper-runtime libjasper1 pngtools libtiff4-dev  libtiff4 libtiffxx0c2 libtiff-tools  libjpeg8 libjpeg8-dev libjpeg8-dbg  ffmpeg libavcodec-dev  libavformat-dev  libgstreamer0.10-0-dbg libgstreamer0.10-0  libgstreamer0.10-dev  libxine1-ffmpeg  libxine-dev libxine1-bin libunicap2 libunicap2-dev libdc1394-22-dev libdc1394-22 libdc1394-utils  swig  libv4l-0 libv4l-dev  python-numpy libswscale-dev libgtk2.0-dev
    (more information on dependencies is available here: http://opencv.willowgarage.com/wiki/InstallGuide%20%3A%20Debian)
  $ get tarball at http://sourceforge.net/projects/opencvlibrary/ and put it in the folder Downloads (or any other)
  $ cd ~/Downloads   
  $ tar -xjvf OpenCV-????.tar.bz2
  $ cd OpenCV-????
  $ mkdir release
  $ cd release
  $ cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_PYTHON_SUPPORT=ON -D BUILD_EXAMPLES=ON ..
  $ make
  $ sudo make install 

* kdev4
  - Open/Import a project
  - Choose the Makefile file at the skinSegmentation folder (will create a skinSegmentation.kdev4 file in this folder)
  - Change Run configuration (Run -> Configure Launchs)
    * Change executable path to YOUR_HOME/skinSegmentation/bin/skinSegmentation
    * Change working directory YOUR_HOME/skinSegmentation/bin) 
   