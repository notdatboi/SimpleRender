#ifndef SIMPLERENDER_SIGNAL_HPP
#define SIMPLERENDER_SIGNAL_HPP
#include <functional>
#include <vector>

namespace sr
{
    template<typename ...Args>
    class Signal
    {
    public:
        void connect(std::function<void(Args...)> slot)
        {
            slots.push_back(slot);
        }

        void operator()(Args... args)
        {
            for(auto& slot : slots)
            {
                slot(args...);
            }
        }

        void disconnect()
        {
            slots.clear();
        }
    private:
        std::vector<std::function<void(Args...)>> slots;
    };
}

#endif