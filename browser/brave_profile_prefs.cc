/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_profile_prefs.h"

#include <string>

#include "brave/browser/new_tab/new_tab_shows_options.h"

#include "brave/browser/brave_shields/brave_shields_web_contents_observer.h"
#include "brave/browser/ethereum_remote_client/buildflags/buildflags.h"
#include "brave/browser/search/ntp_utils.h"
#include "brave/browser/themes/brave_dark_mode_utils.h"
#include "brave/browser/ui/omnibox/brave_omnibox_client_impl.h"
#include "brave/components/binance/browser/buildflags/buildflags.h"
#include "brave/components/brave_adaptive_captcha/buildflags/buildflags.h"
#include "brave/components/brave_ads/browser/ads_p2a.h"
#include "brave/components/brave_perf_predictor/browser/p3a_bandwidth_savings_tracker.h"
#include "brave/components/brave_perf_predictor/browser/perf_predictor_tab_helper.h"
#include "brave/components/brave_rewards/common/pref_names.h"
#include "brave/components/brave_search/browser/brave_search_default_host.h"
#include "brave/components/brave_search/common/brave_search_utils.h"
#include "brave/components/brave_search_conversion/utils.h"
#include "brave/components/brave_shields/browser/brave_farbling_service.h"
#include "brave/components/brave_shields/browser/brave_shields_p3a.h"
#include "brave/components/brave_shields/common/pref_names.h"
#include "brave/components/brave_sync/brave_sync_prefs.h"
#include "brave/components/brave_today/browser/brave_news_controller.h"
#include "brave/components/brave_today/common/features.h"
#include "brave/components/brave_wallet/browser/brave_wallet_prefs.h"
#include "brave/components/brave_wayback_machine/buildflags.h"
#include "brave/components/brave_webtorrent/browser/buildflags/buildflags.h"
#include "brave/components/constants/pref_names.h"
#include "brave/components/crypto_dot_com/browser/buildflags/buildflags.h"
#include "brave/components/de_amp/common/pref_names.h"
#include "brave/components/ftx/browser/buildflags/buildflags.h"
#include "brave/components/gemini/browser/buildflags/buildflags.h"
#include "brave/components/ipfs/buildflags/buildflags.h"
#include "brave/components/ntp_background_images/buildflags/buildflags.h"
#include "brave/components/omnibox/browser/brave_omnibox_prefs.h"
#include "brave/components/search_engines/brave_prepopulated_engines.h"
#include "brave/components/sidebar/buildflags/buildflags.h"
#include "brave/components/speedreader/common/buildflags.h"
#include "brave/components/tor/buildflags/buildflags.h"
#include "brave/components/translate/core/common/buildflags.h"
#include "build/build_config.h"
#include "chrome/browser/prefetch/pref_names.h"
#include "chrome/browser/prefetch/prefetch_prefs.h"
#include "chrome/browser/prefs/session_startup_pref.h"
#include "chrome/browser/ui/webui/new_tab_page/ntp_pref_names.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/pref_names.h"
#include "components/autofill/core/common/autofill_prefs.h"
#include "components/content_settings/core/common/pref_names.h"
#include "components/embedder_support/pref_names.h"
#include "components/gcm_driver/gcm_buildflags.h"
#include "components/password_manager/core/common/password_manager_pref_names.h"
#include "components/policy/core/common/policy_pref_names.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/privacy_sandbox/privacy_sandbox_prefs.h"
#include "components/safe_browsing/core/common/safe_browsing_prefs.h"
#include "components/search_engines/search_engines_pref_names.h"
#include "components/signin/public/base/signin_pref_names.h"
#include "components/sync/base/pref_names.h"
#include "extensions/buildflags/buildflags.h"
#include "third_party/widevine/cdm/buildflags.h"

#if BUILDFLAG(BRAVE_ADAPTIVE_CAPTCHA_ENABLED)
#include "brave/components/brave_adaptive_captcha/brave_adaptive_captcha_service.h"
#endif

#if BUILDFLAG(ENABLE_BRAVE_WEBTORRENT)
#include "brave/components/brave_webtorrent/browser/webtorrent_util.h"
#endif

#if BUILDFLAG(ENABLE_WIDEVINE)
#include "brave/browser/widevine/widevine_utils.h"
#endif

#if BUILDFLAG(ENABLE_BRAVE_WAYBACK_MACHINE)
#include "brave/components/brave_wayback_machine/pref_names.h"
#endif

#if BUILDFLAG(ETHEREUM_REMOTE_CLIENT_ENABLED)
#include "brave/browser/ethereum_remote_client/ethereum_remote_client_constants.h"
#include "brave/browser/ethereum_remote_client/pref_names.h"
#endif

#if BUILDFLAG(ENABLE_IPFS)
#include "brave/components/ipfs/ipfs_service.h"
#endif

#if BUILDFLAG(GEMINI_ENABLED)
#include "brave/components/gemini/browser/pref_names.h"
#endif

#if !BUILDFLAG(USE_GCM_FROM_PLATFORM)
#include "brave/browser/gcm_driver/brave_gcm_utils.h"
#endif

#if BUILDFLAG(ENABLE_SPEEDREADER)
#include "brave/components/speedreader/speedreader_service.h"
#endif

#if BUILDFLAG(CRYPTO_DOT_COM_ENABLED)
#include "brave/components/crypto_dot_com/browser/crypto_dot_com_pref_utils.h"
#include "brave/components/crypto_dot_com/common/pref_names.h"
#endif

#if BUILDFLAG(ENABLE_FTX)
#include "brave/components/ftx/browser/ftx_pref_utils.h"
#endif

#if BUILDFLAG(ENABLE_TOR)
#include "brave/components/tor/tor_profile_service.h"
#endif

#if BUILDFLAG(IS_ANDROID)
#include "components/feed/core/common/pref_names.h"
#include "components/feed/core/shared_prefs/pref_names.h"
#include "components/ntp_tiles/pref_names.h"
#include "components/translate/core/browser/translate_pref_names.h"
#endif

#if !BUILDFLAG(IS_ANDROID)
#include "brave/browser/search_engines/search_engine_provider_util.h"
#include "brave/browser/ui/tabs/brave_tab_prefs.h"
#include "brave/components/brave_private_new_tab_ui/common/pref_names.h"
#endif

#if BUILDFLAG(ENABLE_SIDEBAR)
#include "brave/components/sidebar/sidebar_service.h"
#endif

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "extensions/common/feature_switch.h"
using extensions::FeatureSwitch;
#endif

#if BUILDFLAG(ENABLE_BRAVE_TRANSLATE_GO)
#include "brave/browser/translate/brave_translate_prefs_migration.h"
#endif

#if BUILDFLAG(ENABLE_CUSTOM_BACKGROUND)
#include "brave/browser/ntp_background/ntp_background_prefs.h"
#endif

namespace brave {

void RegisterProfilePrefsForMigration(
    user_prefs::PrefRegistrySyncable* registry) {
#if BUILDFLAG(ENABLE_WIDEVINE)
  RegisterWidevineProfilePrefsForMigration(registry);
#endif

  dark_mode::RegisterBraveDarkModePrefsForMigration(registry);
#if !BUILDFLAG(IS_ANDROID)
  new_tab_page::RegisterNewTabPagePrefsForMigration(registry);

  // Added 06/2022
  brave::RegisterSearchEngineProviderPrefsForMigration(registry);

  // Added 10/2022
  registry->RegisterIntegerPref(kDefaultBrowserLaunchingCount, 0);
#endif
  brave_wallet::RegisterProfilePrefsForMigration(registry);

  // Restore "Other Bookmarks" migration
  registry->RegisterBooleanPref(kOtherBookmarksMigrated, false);

  // Added 05/2021
  registry->RegisterBooleanPref(kBraveTodayIntroDismissed, false);

  // Added 01/2022
  registry->RegisterBooleanPref(brave_rewards::prefs::kHideButton, false);

#if BUILDFLAG(ENABLE_EXTENSIONS)
  // Added 11/2022
  registry->RegisterBooleanPref(kDontAskEnableWebDiscovery, false);
  registry->RegisterIntegerPref(kBraveSearchVisitCount, 0);
#endif
}

void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry) {
  brave_shields::BraveShieldsWebContentsObserver::RegisterProfilePrefs(
      registry);

  brave_perf_predictor::PerfPredictorTabHelper::RegisterProfilePrefs(registry);
  brave_perf_predictor::P3ABandwidthSavingsTracker::RegisterProfilePrefs(
      registry);

  // appearance
  registry->RegisterBooleanPref(kShowBookmarksButton, true);
  registry->RegisterBooleanPref(kShowSidePanelButton, true);
  registry->RegisterBooleanPref(kLocationBarIsWide, false);
  registry->RegisterBooleanPref(brave_rewards::prefs::kShowButton, true);
  registry->RegisterBooleanPref(kMRUCyclingEnabled, false);
  registry->RegisterBooleanPref(kTabsSearchShow, true);

  brave_sync::Prefs::RegisterProfilePrefs(registry);

  brave_shields::RegisterShieldsP3AProfilePrefs(registry);

  brave_news::BraveNewsController::RegisterProfilePrefs(registry);

  // TODO(shong): Migrate this to local state also and guard in ENABLE_WIDEVINE.
  // We don't need to display "don't ask widevine prompt option" in settings
  // if widevine is disabled.
  // F/u issue: https://github.com/brave/brave-browser/issues/7000
  registry->RegisterBooleanPref(kAskWidevineInstall, true);

  // Default Brave shields
  registry->RegisterBooleanPref(kNoScriptControlType, false);
  registry->RegisterBooleanPref(kAdControlType, true);
  registry->RegisterBooleanPref(kShieldsAdvancedViewEnabled, false);

#if !BUILDFLAG(USE_GCM_FROM_PLATFORM)
  // PushMessaging
  gcm::RegisterGCMProfilePrefs(registry);
#endif

  registry->RegisterBooleanPref(kShieldsStatsBadgeVisible, true);
  registry->RegisterBooleanPref(kGoogleLoginControlType, true);
  registry->RegisterBooleanPref(brave_shields::prefs::kFBEmbedControlType,
                                true);
  registry->RegisterBooleanPref(brave_shields::prefs::kTwitterEmbedControlType,
                                true);
  registry->RegisterBooleanPref(brave_shields::prefs::kLinkedInEmbedControlType,
                                false);

#if BUILDFLAG(ENABLE_IPFS)
  ipfs::IpfsService::RegisterProfilePrefs(registry);
#endif

  // WebTorrent
#if BUILDFLAG(ENABLE_BRAVE_WEBTORRENT)
  webtorrent::RegisterProfilePrefs(registry);
#endif

  // wayback machine
#if BUILDFLAG(ENABLE_BRAVE_WAYBACK_MACHINE)
  registry->RegisterBooleanPref(kBraveWaybackMachineEnabled, true);
#endif

#if BUILDFLAG(BRAVE_ADAPTIVE_CAPTCHA_ENABLED)
  brave_adaptive_captcha::BraveAdaptiveCaptchaService::RegisterProfilePrefs(
      registry);
#endif

#if BUILDFLAG(IS_ANDROID)
  registry->RegisterBooleanPref(kDesktopModeEnabled, false);
  registry->RegisterBooleanPref(kPlayYTVideoInBrowserEnabled, true);
  registry->RegisterBooleanPref(kBackgroundVideoPlaybackEnabled, false);
  registry->RegisterBooleanPref(kSafetynetCheckFailed, false);
  // clear default popular sites
  registry->SetDefaultPrefValue(ntp_tiles::prefs::kPopularSitesJsonPref,
                                base::Value(base::Value::Type::LIST));
  // Disable NTP suggestions
  feed::RegisterProfilePrefs(registry);
  registry->RegisterBooleanPref(feed::prefs::kEnableSnippets, false);
  registry->RegisterBooleanPref(feed::prefs::kArticlesListVisible, false);

  // Explicitly disable safe browsing extended reporting by default in case they
  // change it in upstream.
  registry->SetDefaultPrefValue(prefs::kSafeBrowsingScoutReportingEnabled,
                                base::Value(false));
#endif

  // Hangouts
  registry->RegisterBooleanPref(kHangoutsEnabled, true);

  // Restore last profile on restart
  registry->SetDefaultPrefValue(
      prefs::kRestoreOnStartup,
      base::Value(SessionStartupPref::kPrefValueLast));

  // Show download prompt by default
  registry->SetDefaultPrefValue(prefs::kPromptForDownload, base::Value(true));

  // Not using chrome's web service for resolving navigation errors
  registry->SetDefaultPrefValue(embedder_support::kAlternateErrorPagesEnabled,
                                base::Value(false));

  // Disable safebrowsing reporting
  registry->SetDefaultPrefValue(
      prefs::kSafeBrowsingExtendedReportingOptInAllowed, base::Value(false));

  // Disable search suggestion
  registry->SetDefaultPrefValue(prefs::kSearchSuggestEnabled,
                                base::Value(false));

  // Disable "Use a prediction service to load pages more quickly"
  registry->SetDefaultPrefValue(
      prefetch::prefs::kNetworkPredictionOptions,
      base::Value(
          static_cast<int>(prefetch::NetworkPredictionOptions::kDisabled)));

  // Disable cloud print
  // Cloud Print: Don't allow this browser to act as Cloud Print server
  registry->SetDefaultPrefValue(prefs::kCloudPrintProxyEnabled,
                                base::Value(false));
  // Cloud Print: Don't allow jobs to be submitted
  registry->SetDefaultPrefValue(prefs::kCloudPrintSubmitEnabled,
                                base::Value(false));

  // Disable default webstore icons in topsites or apps.
  registry->SetDefaultPrefValue(prefs::kHideWebStoreIcon, base::Value(true));

  // Disable Chromium's privacy sandbox
  registry->SetDefaultPrefValue(prefs::kPrivacySandboxApisEnabled,
                                base::Value(false));
  registry->SetDefaultPrefValue(prefs::kPrivacySandboxApisEnabledV2,
                                base::Value(false));

  // Importer: selected data types
  registry->RegisterBooleanPref(kImportDialogExtensions, true);
  registry->RegisterBooleanPref(kImportDialogPayments, true);

  // IPFS companion extension
  registry->RegisterBooleanPref(kIPFSCompanionEnabled, false);

  // New Tab Page
  registry->RegisterBooleanPref(kNewTabPageShowClock, true);
  registry->RegisterStringPref(kNewTabPageClockFormat, "");
  registry->RegisterBooleanPref(kNewTabPageShowStats, true);
  registry->RegisterBooleanPref(kNewTabPageShowRewards, true);
  registry->RegisterBooleanPref(kNewTabPageShowBinance, false);
  registry->RegisterBooleanPref(kNewTabPageShowBraveTalk, true);
  registry->RegisterBooleanPref(kNewTabPageShowGemini, false);
  registry->RegisterBooleanPref(kNewTabPageHideAllWidgets, false);

// Private New Tab Page
#if !BUILDFLAG(IS_ANDROID)
  brave_private_new_tab::prefs::RegisterProfilePrefs(registry);
#endif

  registry->RegisterIntegerPref(
      kNewTabPageShowsOptions,
      static_cast<int>(NewTabPageShowsOptions::kDashboard));

#if BUILDFLAG(ENABLE_CUSTOM_BACKGROUND)
  NTPBackgroundPrefs::RegisterPref(registry);
#endif

#if BUILDFLAG(ETHEREUM_REMOTE_CLIENT_ENABLED)
  registry->RegisterIntegerPref(kERCPrefVersion, 0);
  registry->RegisterStringPref(kERCAES256GCMSivNonce, "");
  registry->RegisterStringPref(kERCEncryptedSeed, "");
  registry->RegisterBooleanPref(kERCOptedIntoCryptoWallets, false);
#endif

  // Brave Wallet
  brave_wallet::RegisterProfilePrefs(registry);

  // Brave Search
  if (brave_search::IsDefaultAPIEnabled()) {
    brave_search::BraveSearchDefaultHost::RegisterProfilePrefs(registry);
  }

  // Restore default behaviour for Android until we figure out if we want this
  // option there.
#if BUILDFLAG(IS_ANDROID)
  bool allow_open_search_engines = true;
#else
  bool allow_open_search_engines = false;
#endif
  registry->RegisterBooleanPref(prefs::kAddOpenSearchEngines,
                                allow_open_search_engines);

  // Binance widget
#if BUILDFLAG(BINANCE_ENABLED)
  registry->RegisterStringPref(kBinanceAccessToken, "");
  registry->RegisterStringPref(kBinanceRefreshToken, "");
#endif

  // Gemini widget
#if BUILDFLAG(GEMINI_ENABLED)
  registry->RegisterStringPref(kGeminiAccessToken, "");
  registry->RegisterStringPref(kGeminiRefreshToken, "");
#endif

  omnibox::RegisterBraveProfilePrefs(registry);

  // Password leak detection should be disabled
  registry->SetDefaultPrefValue(
      password_manager::prefs::kPasswordLeakDetectionEnabled,
      base::Value(false));
  registry->SetDefaultPrefValue(autofill::prefs::kAutofillWalletImportEnabled,
                                base::Value(false));

  // Default search engine version
  registry->RegisterIntegerPref(
      prefs::kBraveDefaultSearchVersion,
      TemplateURLPrepopulateData::kBraveCurrentDataVersion);

#if BUILDFLAG(ENABLE_EXTENSIONS)
  // Web discovery extension, default false
  registry->RegisterBooleanPref(kWebDiscoveryEnabled, false);
#endif

#if BUILDFLAG(ENABLE_SPEEDREADER)
  speedreader::SpeedreaderService::RegisterProfilePrefs(registry);
#endif

  de_amp::RegisterProfilePrefs(registry);

#if BUILDFLAG(CRYPTO_DOT_COM_ENABLED)
  crypto_dot_com::RegisterProfilePrefs(registry);
#endif

#if BUILDFLAG(ENABLE_FTX)
  ftx::RegisterProfilePrefs(registry);
#endif

#if BUILDFLAG(ENABLE_TOR)
  tor::TorProfileService::RegisterProfilePrefs(registry);
#endif

#if BUILDFLAG(ENABLE_SIDEBAR)
  sidebar::SidebarService::RegisterProfilePrefs(registry, chrome::GetChannel());
  // Set false for showing sidebar on left by default.
  registry->SetDefaultPrefValue(prefs::kSidePanelHorizontalAlignment,
                                base::Value(false));
#endif

#if !BUILDFLAG(IS_ANDROID)
  BraveOmniboxClientImpl::RegisterProfilePrefs(registry);
  brave_ads::RegisterP2APrefs(registry);

  // Turn on most visited mode on NTP by default.
  // We can turn customization mode on when we have add-shortcut feature.
  registry->SetDefaultPrefValue(ntp_prefs::kNtpUseMostVisitedTiles,
                                base::Value(true));
  registry->RegisterBooleanPref(kEnableWindowClosingConfirm, true);
  registry->RegisterBooleanPref(kEnableClosingLastTab, true);

  brave_tabs::RegisterBraveProfilePrefs(registry);
#endif

  brave_search_conversion::RegisterPrefs(registry);

  registry->SetDefaultPrefValue(prefs::kEnableMediaRouter, base::Value(false));

  registry->RegisterBooleanPref(kEnableMediaRouterOnRestart, false);

  // Disable Raw sockets API (see github.com/brave/brave-browser/issues/11546).
  registry->SetDefaultPrefValue(
      policy::policy_prefs::kIsolatedAppsDeveloperModeAllowed,
      base::Value(false));

  BraveFarblingService::RegisterProfilePrefs(registry);

  RegisterProfilePrefsForMigration(registry);

#if BUILDFLAG(ENABLE_BRAVE_TRANSLATE_GO)
  translate::RegisterBraveProfilePrefsForMigration(registry);
#endif
}

}  // namespace brave
