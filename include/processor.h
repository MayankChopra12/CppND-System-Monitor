#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor
{
public:
    Processor() : idle_(0), nonIdle_(0){};
    float Utilization();

private:
    // in user Hz
    int idle_;
    int nonIdle_;
};

#endif