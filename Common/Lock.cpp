#include "Lock.h"

/**
 * Constructor
 */
Lock::Lock()
{
    pthread_mutex_init(&_mutex, NULL);
}

/**
 * Destructor
 */
Lock::~Lock()
{
    pthread_mutex_destroy(&_mutex);
}

/**
 * Perform a blocking lock operation.
 */
void Lock::lock()
{
    pthread_mutex_lock(&_mutex);
}

/**
 * Unlock the lock.
 */
void Lock::unlock()
{
    pthread_mutex_unlock(&_mutex);
}

/**
 * Try to obtain a lock.
 * @return true if the lock was obtained, false otherwise
 */
bool Lock::tryLock()
{
    return pthread_mutex_trylock(&_mutex) == 0;
}
