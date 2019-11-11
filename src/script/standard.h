// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2019 Bitcoin Association
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef BITCOIN_SCRIPT_STANDARD_H
#define BITCOIN_SCRIPT_STANDARD_H

#include "script/interpreter.h"
#include "uint256.h"

#include <boost/variant.hpp>

#include <cstdint>

static const bool DEFAULT_ACCEPT_DATACARRIER = true;

class CKeyID;
class CScript;

/** A reference to a CScript: the Hash160 of its serialization (see script.h) */
class CScriptID : public uint160 {
public:
    CScriptID() : uint160() {}
    CScriptID(const CScript &in);
    CScriptID(const uint160 &in) : uint160(in) {}
};

//!< bytes (+1 for OP_RETURN, +2 for the pushdata opcodes)
static const uint64_t DEFAULT_DATA_CARRIER_SIZE = 100000;
extern bool fAcceptDatacarrier;

/**
 * Mandatory script verification flags that all new blocks must comply with for
 * them to be valid. (but old blocks may not comply with) Currently just P2SH,
 * but in the future other flags may be added, such as a soft-fork to enforce
 * strict DER encoding.
 *
 * Failing one of these tests may trigger a DoS ban - see CheckInputs() for
 * details.
 */
static const uint32_t MANDATORY_SCRIPT_VERIFY_FLAGS =
    SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC |
    SCRIPT_ENABLE_SIGHASH_FORKID | SCRIPT_VERIFY_LOW_S | SCRIPT_VERIFY_NULLFAIL;

enum txnouttype {
    TX_NONSTANDARD,
    // 'standard' transaction types:
    TX_PUBKEY,
    TX_PUBKEYHASH,
    TX_SCRIPTHASH,
    TX_MULTISIG,
    TX_NULL_DATA,
};

class CNoDestination {
public:
    friend bool operator==(const CNoDestination &a, const CNoDestination &b) {
        return true;
    }
    friend bool operator<(const CNoDestination &a, const CNoDestination &b) {
        return true;
    }
};

/**
 * A txout script template with a specific destination. It is either:
 *  * CNoDestination: no destination set
 *  * CKeyID: TX_PUBKEYHASH destination
 *  * CScriptID: TX_SCRIPTHASH destination
 *  A CTxDestination is the internal data type encoded in a bitcoin address
 */
typedef boost::variant<CNoDestination, CKeyID, CScriptID> CTxDestination;

const char *GetTxnOutputType(txnouttype t);
bool IsValidDestination(const CTxDestination &dest);

/**
 * Return public keys or hashes from scriptPubKey, for 'standard' transaction
 * types.
 * For correctly identifiying TX_NULL_DATA output, you must provide info if Genesis
 * update rules are active for this scriptPubKey (and NOT if the Genesis rules are active for transactions spending this ouput) 
 * Use SolverNoData if you do not care about differentiating between TX_NULL_DATA and TX_NONSTANDARD
 */
bool SolverWithData(const CScript& scriptPubKey, bool isGenesisEnabled, txnouttype& typeRet,
    std::vector<std::vector<uint8_t>>& vSolutionsRet);

/* Use this version of Solver when you are interested only if we know how to spend
 * provided output script and you do not care about data (TX_NULL_DATA) outputs.
 *
 * This version does not try to identify TX_NULL_DATA output - it will return
 * TX_NONSTANDARD and false for such outputs.
 *
 * The meaning of OP_RETURN has changed in the Genesis upgrade and to correctly
 * identify them  you need to provide height of the output - use SolverWithData() to do so.
 */
bool SolverNoData(const CScript& scriptPubKey, txnouttype& typeRet,
    std::vector<std::vector<uint8_t>>& vSolutionsRet);

bool ExtractDestination(const CScript &scriptPubKey,
                        CTxDestination &addressRet);
bool ExtractDestinations(const CScript &scriptPubKey, bool isGenesisEnabled, txnouttype &typeRet,
                         std::vector<CTxDestination> &addressRet,
                         int &nRequiredRet);

CScript GetScriptForDestination(const CTxDestination &dest);
CScript GetScriptForRawPubKey(const CPubKey &pubkey);
CScript GetScriptForMultisig(int nRequired, const std::vector<CPubKey> &keys);

#endif // BITCOIN_SCRIPT_STANDARD_H
