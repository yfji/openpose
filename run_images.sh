export PATH=/home/yfji/GitProjects/openpose-master/3rdparty/caffe/build/lib:$PATH		#for compilation and link
export LD_LIBRARY_PATH=/usr/local/lib/opencv_310:$LD_LIBRARY_PATH				#for run
./build/examples/openpose/openpose_demo.bin --image_dir examples/images/ --num_gpu 6
