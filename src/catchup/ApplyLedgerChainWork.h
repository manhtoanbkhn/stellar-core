// Copyright 2015 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "work/Work.h"

#include "herder/TxSetFrame.h"
#include "util/XDRStream.h"
#include "xdr/Stellar-SCP.h"
#include "xdr/Stellar-ledger.h"

namespace stellar
{

struct LedgerHeaderHistoryEntry;
class TmpDir;

/**
 * This class is responsible for applying transactions stored in files on
 * temporary directory (downloadDir) to local ledger. It requires two sets of
 * files - ledgers and transactions - int .xdr format. Transaction files are
 * used to read transactions that will be used and ledger files are used to
 * check if ledger hashes are matching.
 *
 * In each run it skips or applies transactions from one ledger. Skipping occurs
 * when ledger to by applied is older than LCL from local ledger. At LCL
 * boundary checks are made
 * to confirm that ledgers from files are knot up with LCL. If everything is OK,
 * an apply ledger operation is performed. Then another check is made - if new
 * local ledger matches corresponding ledger from file.
 *
 * Contructor of this class takes some important parameters:
 * * downloadDir - directory containing ledger and transaction files
 * * range - range of ledgers to apply (low boundary can overlap with local
 * history)
 * * lastApplied - reference to last applied ledger header (which is LCL)
 */
class ApplyLedgerChainWork : public Work
{
    TmpDir const& mDownloadDir;
    uint32_t mFirstSeq;
    uint32_t mCurrSeq;
    uint32_t mLastSeq;
    XDRInputFileStream mHdrIn;
    XDRInputFileStream mTxIn;
    TransactionHistoryEntry mTxHistoryEntry;
    LedgerHeaderHistoryEntry& mLastApplied;

    TxSetFramePtr getCurrentTxSet();
    void openCurrentInputFiles();
    bool applyHistoryOfSingleLedger();

  public:
    ApplyLedgerChainWork(Application& app, WorkParent& parent,
                         TmpDir const& downloadDir, uint32_t first,
                         uint32_t last, LedgerHeaderHistoryEntry& lastApplied);
    std::string getStatus() const override;
    void onReset() override;
    void onStart() override;
    void onRun() override;
    Work::State onSuccess() override;
};
}
