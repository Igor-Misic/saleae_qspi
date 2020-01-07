#include "QSPIAnalyzerSettings.h"
#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

QSPIAnalyzerSettings::QSPIAnalyzerSettings()
:	mEnableChannel(UNDEFINED_CHANNEL),
	mClockChannel(UNDEFINED_CHANNEL),
	mDQ0Channel(UNDEFINED_CHANNEL),
	mDQ1Channel(UNDEFINED_CHANNEL),
	mDQ2Channel(UNDEFINED_CHANNEL),
	mDQ3Channel(UNDEFINED_CHANNEL),
	mClockInactiveState(BIT_LOW),
	mModeState(1),
	mDummyCycles(8),
	mAddressSize(3)

{

	mEnableChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mEnableChannelInterface->SetTitleAndTooltip("Enable", "Chip Select");
	mEnableChannelInterface->SetChannel(mEnableChannel);

	mClockChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mClockChannelInterface->SetTitleAndTooltip("Clock", "Clock");
	mClockChannelInterface->SetChannel(mClockChannel);

	//mEnableChannelInterface->SetSelectionOfNoneIsAllowed(true);
	mDQ0ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mDQ0ChannelInterface->SetTitleAndTooltip("DQ0", "Data 0");
	mDQ0ChannelInterface->SetChannel(mDQ0Channel);

	mDQ1ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mDQ1ChannelInterface->SetTitleAndTooltip("DQ1", "Data 1");
	mDQ1ChannelInterface->SetChannel(mDQ1Channel);

	mDQ2ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mDQ2ChannelInterface->SetTitleAndTooltip("DQ2", "Data 2");
	mDQ2ChannelInterface->SetChannel(mDQ2Channel);
	mDQ2ChannelInterface->SetSelectionOfNoneIsAllowed(true);

	mDQ3ChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mDQ3ChannelInterface->SetTitleAndTooltip("DQ3", "Data 3");
	mDQ3ChannelInterface->SetChannel(mDQ3Channel);
	mDQ3ChannelInterface->SetSelectionOfNoneIsAllowed(true);


	mClockInactiveStateInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mClockInactiveStateInterface->SetTitleAndTooltip("Clock Polarity", "");
	mClockInactiveStateInterface->AddNumber(BIT_LOW, "Clock is Low when inactive (CPOL = 0, CPHA = 0)", "CPOL = 0 (Clock Polarity), CPHA = 0 (Clock Phase)");
	mClockInactiveStateInterface->AddNumber(BIT_HIGH, "Clock is High when inactive (CPOL = 1, CPHA = 1)", "CPOL = 1 (Clock Polarity), CPHA = 1 (Clock Phase)");
	mClockInactiveStateInterface->SetNumber(mClockInactiveState);

	mModeStateInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mModeStateInterface->SetTitleAndTooltip("SPI Mode", "");
	mModeStateInterface->AddNumber(1, "Extended", "Extended mode (uses DQ0 for command and DQ[3:0] to data depending on command)");
	mModeStateInterface->AddNumber(2, "Dual", "Dual mode (uses DQ[1:0])");
	mModeStateInterface->AddNumber(3, "Quad", "Quad mode (uses DQ[3:0])");
	mModeStateInterface->SetNumber(mModeState);

	mDummyCyclesInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mDummyCyclesInterface->SetTitleAndTooltip("# Dummy Clock Cycles", "");
	for (U32 i = 1; i <= 15; i++)
	{
		std::stringstream ss;

		ss << i;

		mDummyCyclesInterface->AddNumber(i, ss.str().c_str(), "");
	}
	mDummyCyclesInterface->SetNumber(mDummyCycles);

	mAddressSizeInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mAddressSizeInterface->SetTitleAndTooltip("Address Size (bytes)", "");
	mAddressSizeInterface->AddNumber(0, "None", "None byte address");
	mAddressSizeInterface->AddNumber(1, "One", "One byte address");
	mAddressSizeInterface->AddNumber(2, "Two", "Two byte addresses");
	mAddressSizeInterface->AddNumber(3, "Three", "Three byte addresses");
	mAddressSizeInterface->AddNumber(4, "Four", "Four byte addresses");
	mAddressSizeInterface->SetNumber(mAddressSize);


	AddInterface(mEnableChannelInterface.get());
	AddInterface(mClockChannelInterface.get());
	AddInterface(mDQ0ChannelInterface.get());
	AddInterface(mDQ1ChannelInterface.get());
	AddInterface(mDQ2ChannelInterface.get());
	AddInterface(mDQ3ChannelInterface.get());
	AddInterface(mClockInactiveStateInterface.get());
	AddInterface(mModeStateInterface.get());
	AddInterface(mDummyCyclesInterface.get());
	AddInterface(mAddressSizeInterface.get());


	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel(mEnableChannel, "CS", false);
	AddChannel(mClockChannel, "CLK", false);
	AddChannel(mDQ0Channel, "D0", false);
	AddChannel(mDQ1Channel, "D1", false);
	AddChannel(mDQ2Channel, "D2", false);
	AddChannel(mDQ3Channel, "D3", false);
}

QSPIAnalyzerSettings::~QSPIAnalyzerSettings()
{
}

bool QSPIAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel enable = mEnableChannelInterface->GetChannel();
	Channel clock = mClockChannelInterface->GetChannel();
	Channel dq0 = mDQ0ChannelInterface->GetChannel();
	Channel dq1 = mDQ1ChannelInterface->GetChannel();
	Channel dq2 = mDQ2ChannelInterface->GetChannel();
	Channel dq3 = mDQ3ChannelInterface->GetChannel();

	std::vector<Channel> channels;
	channels.push_back(enable);
	channels.push_back(clock);
	channels.push_back(dq0);
	channels.push_back(dq1);
	channels.push_back(dq2);
	channels.push_back(dq3);

	if (AnalyzerHelpers::DoChannelsOverlap(&channels[0], channels.size()) == true)
	{
		SetErrorText("Please select different channels for each input.");
		return false;
	}

	if ((dq0 == UNDEFINED_CHANNEL) && (dq1 == UNDEFINED_CHANNEL) && (dq2 == UNDEFINED_CHANNEL) && (dq3 == UNDEFINED_CHANNEL) && (clock == UNDEFINED_CHANNEL) && (enable == UNDEFINED_CHANNEL))
	{
		SetErrorText("Please select at least one input for each channel.");
		return false;
	}


	mEnableChannel = mEnableChannelInterface->GetChannel();
	mClockChannel = mClockChannelInterface->GetChannel();
	mDQ0Channel = mDQ0ChannelInterface->GetChannel();
	mDQ1Channel = mDQ1ChannelInterface->GetChannel();
	mDQ2Channel = mDQ2ChannelInterface->GetChannel();
	mDQ3Channel = mDQ3ChannelInterface->GetChannel();

	mClockInactiveState = (BitState) U32(mClockInactiveStateInterface->GetNumber());
	mModeState = U32(mModeStateInterface->GetNumber());
	mDummyCycles = U32(mDummyCyclesInterface->GetNumber());
	mAddressSize = U32(mAddressSizeInterface->GetNumber());

	ClearChannels();
	AddChannel(mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL);
	AddChannel(mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL);
	AddChannel(mDQ0Channel, "DQ0", mDQ0Channel != UNDEFINED_CHANNEL);
	AddChannel(mDQ1Channel, "DQ1", mDQ1Channel != UNDEFINED_CHANNEL);
	AddChannel(mDQ2Channel, "DQ2", mDQ2Channel != UNDEFINED_CHANNEL);
	AddChannel(mDQ3Channel, "DQ3", mDQ3Channel != UNDEFINED_CHANNEL);

	return true;
}

void QSPIAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mEnableChannelInterface->SetChannel(mEnableChannel);
	mClockChannelInterface->SetChannel(mClockChannel);
	mDQ0ChannelInterface->SetChannel(mDQ0Channel);
	mDQ1ChannelInterface->SetChannel(mDQ1Channel);
	mDQ2ChannelInterface->SetChannel(mDQ2Channel);
	mDQ3ChannelInterface->SetChannel(mDQ3Channel);
	mClockInactiveStateInterface->SetNumber(mClockInactiveState);
	mModeStateInterface->SetNumber(mModeState);
	mDummyCyclesInterface->SetNumber(mDummyCycles);
	mAddressSizeInterface->SetNumber(mAddressSize);
}

void QSPIAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );


	text_archive >> mEnableChannel;
	text_archive >> mClockChannel;
	text_archive >> mDQ0Channel;
	text_archive >> mDQ1Channel;
	text_archive >> mDQ2Channel;
	text_archive >> mDQ3Channel;
	text_archive >> *(U32*)&mClockInactiveState;
	text_archive >> *(U32*)&mModeState;
	text_archive >> *(U32*)&mDummyCycles;
	text_archive >> *(U32*)&mAddressSize;

	ClearChannels();
	AddChannel(mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL);
	AddChannel(mClockChannel, "CLOCK", mClockChannel != UNDEFINED_CHANNEL);
	AddChannel(mDQ0Channel, "DQ0", mDQ0Channel != UNDEFINED_CHANNEL);
	AddChannel(mDQ1Channel, "DQ1", mDQ1Channel != UNDEFINED_CHANNEL);
	AddChannel(mDQ2Channel, "DQ2", mDQ2Channel != UNDEFINED_CHANNEL);
	AddChannel(mDQ3Channel, "DQ3", mDQ3Channel != UNDEFINED_CHANNEL);

	UpdateInterfacesFromSettings();
}

const char* QSPIAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mEnableChannel;
	text_archive << mClockChannel;
	text_archive << mDQ0Channel;
	text_archive << mDQ1Channel;
	text_archive << mDQ2Channel;
	text_archive << mDQ3Channel;
	text_archive << mClockInactiveState;
	text_archive << mModeState;
	text_archive << mDummyCycles;
	text_archive << mAddressSize;

	return SetReturnString( text_archive.GetString() );
}
