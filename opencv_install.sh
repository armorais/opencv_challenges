echo deb http://security.ubuntu.com/ubuntu xenial-security main | sudo tee -a /etc/apt/sources.list

sudo apt update

sudo apt-get install build-essential
sudo apt-get install libqt4-dev libqt4-opengl-dev
# sudo apt-get install qt5base-dev libqt5opengl5-dev # Utilizado no Sparky Linux lxQt, que usa qt5
sudo apt-get install libavcodec-dev openexr libopenexr-dev libatlas-base-dev libv4l-dev libx264-dev libxvidcore-dev
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
#git clone https://github.com/opencv/opencv.git
cd opencv
mkdir release && cd release
cmake 	-D CMAKE_BUILD_TYPE=RELEASE \
		-D CMAKE_INSTALL_PREFIX=/opt/opencv \
		-D INSTALL_C_EXAMPLES=OFF \
		-D INSTALL_PYTHON_EXAMPLES=ON \
		-D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
    	-D BUILD_opencv_cvv=OFF \
		-D WITH_TBB=ON \
		-D WITH_V4L=ON \
		-D WITH_QT=ON \
		-D WITH_OPENGL=ON \
		-D WITH_GDAL=ON \
		-D WITH_XINE=ON \
		-D BUILD_EXAMPLES=ON ..
		 
make
sudo make install
