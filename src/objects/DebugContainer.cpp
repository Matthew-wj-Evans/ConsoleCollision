#ifndef DEBUG_CONTAINER_CPP
#define DEBUG_CONTAINER_CPP

class DebugContainer
{
private:
    int _deletionCount;
    int _creationCount;

public:
    DebugContainer() : _creationCount(0), _deletionCount(0) {};
    void CreateInc() { _creationCount++; };
    void DeleteInc() { _deletionCount++; };
    int GetCreateCount() { return _creationCount; };
    int GetDeleteCount() { return _deletionCount; };
};

#endif