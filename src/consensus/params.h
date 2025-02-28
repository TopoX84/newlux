// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <uint256.h>
#include <limits>

namespace Consensus {

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    int64_t nTimeout;

    /** Constant for nTimeout very far in the future. */
    static constexpr int64_t NO_TIMEOUT = std::numeric_limits<int64_t>::max();

    /** Special value for nStartTime indicating that the deployment is always active.
     *  This is useful for testing, as it means tests don't need to deal with the activation
     *  process (which takes at least 3 BIP9 intervals). Only tests that specifically test the
     *  behaviour during activation cannot use this. */
    static constexpr int64_t ALWAYS_ACTIVE = -1;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    int nSubsidyHalvingIntervalV2;
    /* Block hash that is excepted from BIP16 enforcement */
    uint256 BIP16Exception;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    uint256 BIP34Hash;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
    /** Block height at which CSV (BIP68, BIP112 and BIP113) becomes active */
    int CSVHeight;
    /** Block height at which Segwit (BIP141, BIP143 and BIP147) becomes active.
     * Note that segwit v0 script rules are enforced on all blocks except the
     * BIP 16 exception blocks. */
    int SegwitHeight;
    /** Don't warn about unknown BIP 9 activations below this height.
     * This prevents us from warning about the CSV and segwit activations. */
    int MinBIP9WarningHeight;
    /** Block height at which QIP5 becomes active */
    int QIP5Height;
    /** Block height at which QIP6 becomes active */
    int QIP6Height;
    /** Block height at which QIP7 becomes active */
    int QIP7Height;
    /** Block height at which QIP9 becomes active */
    int QIP9Height;
    /** Block height at which Offline Staking becomes active */
    int nOfflineStakeHeight;
    /** Block height at which Reduce Block Time becomes active */
    int nReduceBlocktimeHeight;
    /** Block height at which EVM Muir Glacier fork becomes active */
    int nMuirGlacierHeight;
    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPowTargetTimespan / nPowTargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];
    /** Proof of work parameters */
    uint256 powLimit;
    uint256 posLimit;
    uint256 QIP9PosLimit;
    uint256 RBTPosLimit;
    bool fPowAllowMinDifficultyBlocks;
    bool fPowNoRetargeting;
    bool fPoSNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nRBTPowTargetSpacing;
    int64_t nPowTargetTimespan;
    int64_t nPowTargetTimespanV2;
    int64_t nRBTPowTargetTimespan;
    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;
    int nFirstMPoSBlock;
    int nMPoSRewardRecipients;
    int nFixUTXOCacheHFHeight;
    int nEnableHeaderSignatureHeight;
    /** RX2 related variables */
    uint32_t RX2SeedHeight;
    uint32_t RX2SeedInterval;
    /** LWMA variables */
    int64_t lwmaAveragingWindow;
    /** Block sync-checkpoint span*/
    int nCheckpointSpan;
    int nRBTCheckpointSpan;
    uint160 delegationsAddress;
    int nLastMPoSBlock;
    uint32_t nStakeTimestampMask;
    uint32_t nRBTStakeTimestampMask;
    int64_t nBlocktimeDownscaleFactor;
    /** Coinbase transaction outputs can only be spent after this number of new blocks (network rule) */
    int nCoinbaseMaturity;
    int nRBTCoinbaseMaturity;
    int64_t DifficultyAdjustmentInterval(int height) const
    {
        int64_t targetTimespan = TargetTimespan(height);
        int64_t targetSpacing = TargetSpacing(height);
        return targetTimespan / targetSpacing;
    }
    int64_t StakeTimestampMask(int height) const
    {
        return height < nReduceBlocktimeHeight ? nStakeTimestampMask : nRBTStakeTimestampMask;
    }
    int SubsidyHalvingInterval(int height) const
    {
        return height < nReduceBlocktimeHeight ? nSubsidyHalvingInterval : nSubsidyHalvingIntervalV2;
    }
    int64_t BlocktimeDownscaleFactor(int height) const
    {
        return height < nReduceBlocktimeHeight ? 1 : nBlocktimeDownscaleFactor;
    }
    int64_t TargetSpacing(int height) const
    {
        return height < nReduceBlocktimeHeight ? nPowTargetSpacing : nRBTPowTargetSpacing;
    }
    int SubsidyHalvingWeight(int height) const
    {

        int blocktimeDownscaleFactor = BlocktimeDownscaleFactor(height);
        int blockCount = height;
        int beforeDownscale = blocktimeDownscaleFactor == 1 ? 0 : nReduceBlocktimeHeight - 1;
        int subsidyHalvingWeight = blockCount - beforeDownscale + beforeDownscale * blocktimeDownscaleFactor;
        return subsidyHalvingWeight;
    }
    int64_t TimestampDownscaleFactor(int height) const
    {
        return height < nReduceBlocktimeHeight ? 1 : (nStakeTimestampMask + 1) / (nRBTStakeTimestampMask + 1);
    }
    int64_t TargetTimespan(int height) const
    {
        return height < QIP9Height ? nPowTargetTimespan : 
            (height < nReduceBlocktimeHeight ? nPowTargetTimespanV2 : nRBTPowTargetTimespan);
    }
    int CheckpointSpan(int height) const
    {
        return height < nReduceBlocktimeHeight ? nCheckpointSpan : nRBTCheckpointSpan;
    }
    int CoinbaseMaturity(int height) const
    {
        return height < nReduceBlocktimeHeight ? nCoinbaseMaturity : nRBTCoinbaseMaturity;
    }
    int MaxCheckpointSpan() const
    {
        return nCheckpointSpan <= nRBTCheckpointSpan ? nRBTCheckpointSpan : nCheckpointSpan;
    }
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
