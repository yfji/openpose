export PATH=/home/yfji/SourceCode/openpose-allserver/3rdparty/caffe/build/lib:$PATH		#for compilation and link
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/cuda/lib64:$LD_LIBRARY_PATH				#for run
./build/examples/openpose/openpose_demo.bin --video single_person.mp4 --num_gpu 1 --model_pose BODY_25
