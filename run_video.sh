export PATH=/home/yfji/GitProjects/openpose-master/3rdparty/caffe/build/lib:$PATH		#for compilation and link
export LD_LIBRARY_PATH=/usr/local/lib/opencv330:$LD_LIBRARY_PATH				#for run
./build/examples/openpose/openpose_demo.bin --video examples/single_person.mp4 --num_gpu 1
