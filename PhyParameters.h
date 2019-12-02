#ifndef INCLUDED_NS3_MURASE_LAB__PHY_PARAMETERS_H__
#define INCLUDED_NS3_MURASE_LAB_WIRELESS_LAN_PHY_PARAMETERS_H__

#include "ns3.h"

#include <string>

// ------------------------------------------------
// Wifi Setting
// ------------------------------------------------
static const std::string kRemoteStationDataMode = "HtMcs7";
  // Transmission mode for data.
  // ---- List
  // ---- ---- IEEE 802.11 g
  //        ErpOfdmRate54Mbps
  //        ErpOfdmRate48Mbps
  //        ErpOfdmRate36Mbps
  //        ErpOfdmRate24Mbps
  //        ErpOfdmRate18Mbps
  //        ErpOfdmRate12Mbps
  //        ErpOfdmRate9Mbps
  //        ErpOfdmRate6Mbps
  //
  // ---- ---- IEEE 802.11 n
  //        HtMcs7
  //             :
  //        HtMcs0

static const std::string kRemoteStationControlMode = "HtMcs0";
  // Transmission mode for control messages.
// ------------------------------------------------


// ------------------------------------------------
// Phy Setting
// ------------------------------------------------
static const ns3::WifiPhyStandard kWifiPhyStandard
  = ns3::WifiPhyStandard::WIFI_PHY_STANDARD_80211g;
  // The standard for a wireless LAN.
  // ---- List
  //   WIFI_PHY_STANDARD_80211a
  //   WIFI_PHY_STANDARD_80211b
  //   WIFI_PHY_STANDARD_80211g
  //   WIFI_PHY_STANDARD_80211_10MHZ
  //   WIFI_PHY_STANDARD_80211_5MHZ
  //   WIFI_PHY_STANDARD_holland
  //   WIFI_PHY_STANDARD_80211n_2_4GHZ
  //   WIFI_PHY_STANDARD_80211n_5GHZ
  //   WIFI_PHY_STANDARD_80211ac
  //   WIFI_PHY_STANDARD_80211ax_2_4GHZa
  //   WIFI_PHY_STANDARD_80211ax_5GHZ
  //   WIFI_PHY_STANDARD_UNSPECIFIED

static const ns3::WifiPhyHelper::SupportedPcapDataLinkTypes kPhyPcapDlt
  = ns3::WifiPhyHelper::SupportedPcapDataLinkTypes::DLT_IEEE802_11_RADIO;
  // Data link type.
  // ---- List
  //   DLT_IEEE802_11
  //   DLT_PRISM_HEADER
  //   DLT_IEEE802_11_RADIO

static const double kPhyEnergyDetectionThresholdDbm = -82.0;
  // Carrer sense threshold.

static const double kPhyCcaMode1ThresholdDbm = -82.0;
  // CCA threshold.

static const double kPhyTxPowerStartDbm = 10.0;
  // 10[dBm](10[mW]) is maximum transmission power of wireless LANs in Japan.
  // Recommend -> 10[dBm]

static const double kPhyTxPowerEndDbm = 10.0;
  // 10[dBm](10[mW]) is maximum transmission power of wireless LANs in Japan.
  // Recommend -> 10[dBm]
// ------------------------------------------------


// ------------------------------------------------
// Log Distance Propagation Model Setting
// ------------------------------------------------
static const double kLogDistancePropagationLossExponent = 3.0;
  // Loss exponent for Log Distance Propagation Loss Model.
  // Higher value means noisy environment.
  // In ideal space, this value is equal to 2.0.
  // Recommend -> 3.0

static const double kLogDistancePropagationLossReferenceDistanceMetre = 1.0;
  // Reference distance for Log Distance Propagation Loss Model.
  // Recommend -> 1.0

static const double kLogDistancePropagationLossReferenceLoss = 40.045997;
  // Reference loss for Log Distance Propagation Loss Model.
  // Recommend -> 40.045997
// ------------------------------------------------


// ------------------------------------------------
// Nakagami Propagation Model Setting
// ------------------------------------------------
static const double kNakagamiPropagationLossDistance1Metre = 80.0;
static const double kNakagamiPropagationLossDistance2Metre = 200.0;
static const double kNakagamiPropagationLossM0 = 1.50;
static const double kNakagamiPropagationLossM1 = 0.75;
static const double kNakagamiPropagationLossM2 = 0.75;
// ------------------------------------------------

#endif // INCLUDED_NS3_MURASE_LAB_PHY_PARAMETERS_H__
