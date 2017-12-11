#ifndef OPENPOSE_THREAD_THREAD_HPP
#define OPENPOSE_THREAD_THREAD_HPP

#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
#include <unistd.h>
#include <condition_variable>
#include <openpose/core/common.hpp>
#include <openpose/thread/subThread.hpp>
#include <openpose/thread/worker.hpp>

namespace op
{
    template<typename TDatums, typename TWorker = std::shared_ptr<Worker<TDatums>>>
    class Thread
    {
    public:
        explicit Thread(const std::shared_ptr<std::atomic<bool>>& isRunningSharedPtr = nullptr);

        // Move constructor
        Thread(Thread&& t);

        // Move assignment
        Thread& operator=(Thread&& t);

        // Destructor
        virtual ~Thread();

        void add(const std::vector<std::shared_ptr<SubThread<TDatums, TWorker>>>& subThreads);

        void add(const std::shared_ptr<SubThread<TDatums, TWorker>>& subThread);

        void exec(const std::shared_ptr<std::atomic<bool>>& isRunningSharedPtr, const std::shared_ptr<std::atomic<bool>>& isBlockingSharedPtr, bool _blockSelf=false);

        void startInThread(const std::shared_ptr<std::atomic<bool>>& isBlockingSharedPtr, bool _blockSelf=false);

        void stopAndJoin();

        inline bool isRunning() const
        {
            return *spIsRunning;
        }
        
        inline bool isBlocking() const
        {
            return *spIsBlocking;
        }

    private:
        std::shared_ptr<std::atomic<bool>> spIsRunning;
        std::shared_ptr<std::atomic<bool>> spIsBlocking;
        
        std::vector<std::shared_ptr<SubThread<TDatums, TWorker>>> mSubThreads;
        std::thread mThread;

		bool blockSelf;
        void initializationOnThread();

        void threadFunction();

        void stop();

        void join();

        DELETE_COPY(Thread);
    };
}





// Implementation
namespace op
{
    template<typename TDatums, typename TWorker>
    Thread<TDatums, TWorker>::Thread(const std::shared_ptr<std::atomic<bool>>& isRunningSharedPtr) :
        spIsRunning{(isRunningSharedPtr != nullptr ? isRunningSharedPtr : std::make_shared<std::atomic<bool>>(false))}
    {
    	spIsBlocking=std::make_shared<std::atomic<bool>>(false);
    	blockSelf=false;
    }

    template<typename TDatums, typename TWorker>
    Thread<TDatums, TWorker>::Thread(Thread<TDatums, TWorker>&& t) :
        spIsRunning{std::make_shared<std::atomic<bool>>(t.spIsRunning->load())},
        spIsBlocking{std::make_shared<std::atomic<bool>>(t.spIsBlocking->load())}  
    {
        std::swap(mSubThreads, t.mSubThreads);
        std::swap(mThread, t.mThread);
        blockSelf=t.blockSelf;
    }

    template<typename TDatums, typename TWorker>
    Thread<TDatums, TWorker>& Thread<TDatums, TWorker>::operator=(Thread<TDatums, TWorker>&& t)
    {
        std::swap(mSubThreads, t.mSubThreads);
        std::swap(mThread, t.mThread);
        spIsRunning = {std::make_shared<std::atomic<bool>>(t.spIsRunning->load())};
        spIsBlocking = {std::make_shared<std::atomic<bool>>(t.spIsBlocking->load())};
        blockSelf=t.blockSelf;
        return *this;
    }

    template<typename TDatums, typename TWorker>
    Thread<TDatums, TWorker>::~Thread()
    {
        try
        {
            log("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            stopAndJoin();
            log("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::add(const std::vector<std::shared_ptr<SubThread<TDatums, TWorker>>>& subThreads)
    {
        for (const auto& subThread : subThreads)
            mSubThreads.emplace_back(subThread);
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::add(const std::shared_ptr<SubThread<TDatums, TWorker>>& subThread)
    {
        add(std::vector<std::shared_ptr<SubThread<TDatums, TWorker>>>{subThread});
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::exec(const std::shared_ptr<std::atomic<bool>>& isRunningSharedPtr, const std::shared_ptr<std::atomic<bool>>& isBlockingSharedPtr, bool _blockSelf)
    {
        try
        {
            stopAndJoin();
            spIsRunning = isRunningSharedPtr;
            spIsBlocking= isBlockingSharedPtr;
            *spIsRunning = {true};
            // *spIsBlocking= {false};
            blockSelf=_blockSelf;
            threadFunction();
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::startInThread(const std::shared_ptr<std::atomic<bool>>& isBlockingSharedPtr, bool _blockSelf)
    {
        try
        {
            log("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            stopAndJoin();
            *spIsRunning = {true};
            spIsBlocking=isBlockingSharedPtr;
            // *spIsBlocking= {false};
            blockSelf=_blockSelf;
            mThread = {std::thread{&Thread::threadFunction, this}};
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::stopAndJoin()
    {
        try
        {
            stop();
            join();
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::initializationOnThread()
    {
        try
        {
            for (auto& subThread : mSubThreads)
                subThread->initializationOnThread();
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::threadFunction()
    {
        try
        {
            log("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            initializationOnThread();

            log("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            while (isRunning())
            {
            	// std::cout<<"Running: "<<blockSelf<<std::endl;
            	if(not blockSelf){
		        	while(*spIsBlocking){	usleep(10);	}
            	}
            	else{
            		;// std::cout<<"I block myself"<<std::endl;
        		}
                bool allSubThreadsClosed = true;
                for (auto& subThread : mSubThreads)
                    allSubThreadsClosed &= !subThread->work();

                if (allSubThreadsClosed)
                {
                    log("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                    stop();
                    break;
                }
            }
            log("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::stop()
    {
        try
        {
            *spIsRunning = {false};
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    template<typename TDatums, typename TWorker>
    void Thread<TDatums, TWorker>::join()
    {
        try
        {
            if (mThread.joinable())
                mThread.join();
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    COMPILE_TEMPLATE_DATUM(Thread);
}

#endif // OPENPOSE_THREAD_THREAD_HPP
