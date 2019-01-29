#include <utility>
#include <string>
#include <algorithm>
#include <list>
#include <vector>

#include "tendisplus/network/session_ctx.h"
#include "tendisplus/utils/invariant.h"

namespace tendisplus {

SessionCtx::SessionCtx()
    :_authed(false),
     _dbId(0),
     _waitlockStore(0),
     _waitlockMode(mgl::LockMode::LOCK_NONE) {
}

bool SessionCtx::authed() const {
    return _authed;
}

uint32_t SessionCtx::getDbId() const {
    return _dbId;
}

void SessionCtx::setDbId(uint32_t dbid) {
    _dbId = dbid;
}

void SessionCtx::setAuthed() {
    _authed = true;
}

void SessionCtx::resetSingleReqCtx() {
    _readPacketCost = 0;
    _processPacketStart = 0;
    _processPacketEnd = 0;
    _sendPacketStart = 0;
    _sendPacketEnd = 0;
}

void SessionCtx::addReadPacketCost(uint64_t cost) {
    _readPacketCost += cost;
}

uint64_t SessionCtx::getReadPacketCost() const {
    return _readPacketCost;
}

void SessionCtx::setProcessPacketStart(uint64_t start) {
    _processPacketStart = start;
}

uint64_t SessionCtx::getProcessPacketStart() const {
    return _processPacketStart;
}

void SessionCtx::setProcessPacketEnd(uint64_t end) {
    _processPacketEnd = end;
}

uint64_t SessionCtx::getProcessPacketEnd() const {
    return _processPacketEnd;
}

void SessionCtx::setSendPacketStart(uint64_t start) {
    _sendPacketStart = start;
}

uint64_t SessionCtx::getSendPacketStart() const {
    return _sendPacketStart;
}

void SessionCtx::setSendPacketEnd(uint64_t end) {
    _sendPacketEnd = end;
}

uint64_t SessionCtx::getSendPacketEnd() const {
    return _sendPacketEnd;
}

void SessionCtx::addLock(StoreLock *lock) {
    std::lock_guard<std::mutex> lk(_mutex);
    _locks.push_back(lock);
}

void SessionCtx::removeLock(StoreLock *lock) {
    std::lock_guard<std::mutex> lk(_mutex);
    for (auto it = _locks.begin(); it != _locks.end(); ++it) {
        if (*it == lock) {
            _locks.erase(it);
            return;
        }
    }
    INVARIANT(0);
}

std::vector<std::string> SessionCtx::getArgsBrief() const {
    std::lock_guard<std::mutex> lk(_mutex);
    return _argsBrief;
}

void SessionCtx::setArgsBrief(const std::vector<std::string>& v) {
    std::lock_guard<std::mutex> lk(_mutex);
    constexpr size_t MAX_SIZE = 8;
    for (size_t i = 0; i < std::min(v.size(), MAX_SIZE); ++i) {
        _argsBrief.push_back(v[i]);
    }
}

void SessionCtx::clearRequestCtx() {
    std::lock_guard<std::mutex> lk(_mutex);
    _argsBrief.clear();
}

void SessionCtx::setWaitLock(uint32_t storeId, mgl::LockMode mode) {
    _waitlockStore = storeId;
    _waitlockMode = mode;
}

SLSP SessionCtx::getWaitlock() const {
    return std::pair<uint32_t, mgl::LockMode>(_waitlockStore, _waitlockMode);
}

std::list<SLSP> SessionCtx::getLockStates() const {
    std::lock_guard<std::mutex> lk(_mutex);
    std::list<SLSP> result;
    for (auto& lk : _locks) {
        result.push_back(
            SLSP(lk->getStoreId(), lk->getMode()));
    }
    return result;
}

}  // namespace tendisplus