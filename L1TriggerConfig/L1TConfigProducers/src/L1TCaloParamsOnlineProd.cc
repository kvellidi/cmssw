#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>

#include "CondTools/L1TriggerExt/interface/L1ConfigOnlineProdBaseExt.h"
#include "CondFormats/L1TObjects/interface/CaloParams.h"
#include "CondFormats/DataRecord/interface/L1TCaloStage2ParamsRcd.h"
#include "CondFormats/DataRecord/interface/L1TCaloParamsO2ORcd.h"
#include "L1Trigger/L1TCommon/interface/TriggerSystem.h"
#include "L1Trigger/L1TCommon/interface/XmlConfigParser.h"
#include "L1Trigger/L1TCommon/interface/ConvertToLUT.h"
#include "L1Trigger/L1TCalorimeter/interface/CaloParamsHelper.h"
#include "OnlineDBqueryHelper.h"

#include "xercesc/util/PlatformUtils.hpp"
using namespace XERCES_CPP_NAMESPACE;

class L1TCaloParamsOnlineProd : public L1ConfigOnlineProdBaseExt<L1TCaloParamsO2ORcd,l1t::CaloParams> {
private:
public:
    virtual std::shared_ptr<l1t::CaloParams> newObject(const std::string& objectKey, const L1TCaloParamsO2ORcd& record) override ;

    L1TCaloParamsOnlineProd(const edm::ParameterSet&);
    ~L1TCaloParamsOnlineProd(void){}
};

bool
readCaloLayer1OnlineSettings(l1t::CaloParamsHelper& paramsHelper, std::map<std::string, l1t::Parameter>& conf, std::map<std::string, l1t::Mask>& ) {
  const char * expectedParams[] = {
    "layer1ECalScaleFactors",
    "layer1HCalScaleFactors",
    "layer1HFScaleFactors",
    "layer1ECalScaleETBins",
    "layer1HCalScaleETBins",
    "layer1HFScaleETBins"
  };
  for (const auto param : expectedParams) {
    if ( conf.find(param) == conf.end() ) {
      std::cerr << "Unable to locate expected CaloLayer1 parameter: " << param << " in L1 settings payload!";
      return false;
    }
  }
  // Layer 1 LUT specification
  paramsHelper.setLayer1ECalScaleFactors((conf["layer1ECalScaleFactors"].getVector<double>()));
  paramsHelper.setLayer1HCalScaleFactors((conf["layer1HCalScaleFactors"].getVector<double>()));
  paramsHelper.setLayer1HFScaleFactors  ((conf["layer1HFScaleFactors"]  .getVector<double>()));
  paramsHelper.setLayer1ECalScaleETBins(conf["layer1ECalScaleETBins"].getVector<int>());
  paramsHelper.setLayer1HCalScaleETBins(conf["layer1HCalScaleETBins"].getVector<int>());
  paramsHelper.setLayer1HFScaleETBins  (conf["layer1HFScaleETBins"]  .getVector<int>());
  paramsHelper.setLayer1ECalScalePhiBins(conf.find("layer1ECalScalePhiBins") != conf.end() ? conf["layer1ECalScalePhiBins"].getVector<unsigned int>() : std::vector<unsigned>(36,0));
  paramsHelper.setLayer1HCalScalePhiBins(conf.find("layer1HCalScalePhiBins") != conf.end() ? conf["layer1HCalScalePhiBins"].getVector<unsigned int>() : std::vector<unsigned>(36,0));
  paramsHelper.setLayer1HFScalePhiBins  (conf.find("layer1HFScalePhiBins") != conf.end() ? conf["layer1HFScalePhiBins"]  .getVector<unsigned int>() : std::vector<unsigned>(36,0));

  return true;
}

bool
readCaloLayer2OnlineSettings(l1t::CaloParamsHelper& paramsHelper, std::map<std::string, l1t::Parameter>& conf, std::map<std::string, l1t::Mask>& ) {
  const char * expectedParams[] = {
    "leptonSeedThreshold",
    "leptonTowerThreshold",
    "pileUpTowerThreshold",
    "jetSeedThreshold",
    "jetMaxEta",
    "HTMHT_maxJetEta",
    "HT_jetThreshold",
    "MHT_jetThreshold",
    "jetEnergyCalibLUT",
    "ETMET_maxTowerEta",
    "ET_energyCalibLUT",
    "ecalET_energyCalibLUT",
    "METX_energyCalibLUT",
    "METY_energyCalibLUT",
    "egammaRelaxationThreshold",
    "egammaMaxEta",
    "egammaEnergyCalibLUT",
    "egammaIsoLUT",
    "tauMaxEta",
    "tauEnergyCalibLUT",
    "tauIsoLUT1",
    "tauIsoLUT2",
    "towerCountThreshold",
    "towerCountMaxEta",
    "ET_towerThreshold",
    "MET_towerThreshold",
    "jetBypassPileUpSub",
    "egammaBypassCuts",
    "egammaHOverECut_iEtaLT15",
    "egammaHOverECut_iEtaGTEq15"
  };
  for (const auto param : expectedParams) {
    if ( conf.find(param) == conf.end() ) {
      std::cerr << "Unable to locate expected CaloLayer2 parameter: " << param << " in L1 settings payload!";
      return false;
    }
  }
  // Layer 2 params specification
  paramsHelper.setEgSeedThreshold((conf["leptonSeedThreshold"].getValue<int>())/2);
  paramsHelper.setTauSeedThreshold((conf["leptonSeedThreshold"].getValue<int>())/2);
  paramsHelper.setEgNeighbourThreshold((conf["leptonTowerThreshold"].getValue<int>())/2);
  paramsHelper.setTauNeighbourThreshold((conf["leptonTowerThreshold"].getValue<int>())/2);
  paramsHelper.setJetSeedThreshold((conf["jetSeedThreshold"].getValue<int>())/2);
  paramsHelper.setJetBypassPUS(conf["jetBypassPileUpSub"].getValue<bool>());
  paramsHelper.setEgBypassEGVetos(conf["egammaBypassCuts"].getValue<bool>());
  paramsHelper.setEgHOverEcutBarrel(conf["egammaHOverECut_iEtaLT15"].getValue<int>());
  paramsHelper.setEgHOverEcutEndcap(conf["egammaHOverECut_iEtaGTEq15"].getValue<int>());


  // Currently not used // paramsHelper.setEgPileupTowerThresh((conf["pileUpTowerThreshold"].getValue<int>())); 
  // Currently not used // paramsHelper.setTauPileupTowerThresh((conf["pileUpTowerThreshold"].getValue<int>())); 
  // Currently not used // paramsHelper.setJetMaxEta((conf["jetMaxEta"].getValue<int>()));
  
  std::vector<int> etSumEtaMax;
  std::vector<int> etSumEtThresh;
  
  etSumEtaMax.push_back(conf["ETMET_maxTowerEta"].getValue<int>());
  etSumEtaMax.push_back(conf["HTMHT_maxJetEta"].getValue<int>());
  etSumEtaMax.push_back(conf["ETMET_maxTowerEta"].getValue<int>());
  etSumEtaMax.push_back(conf["HTMHT_maxJetEta"].getValue<int>());
  etSumEtaMax.push_back(conf["towerCountMaxEta"].getValue<int>());
  
  etSumEtThresh.push_back(conf["ET_towerThreshold"].getValue<int>()/2); // ETT tower threshold
  etSumEtThresh.push_back(conf["HT_jetThreshold"].getValue<int>()/2);
  etSumEtThresh.push_back(conf["MET_towerThreshold"].getValue<int>()/2); // ETM tower threshold
  etSumEtThresh.push_back(conf["MHT_jetThreshold"].getValue<int>()/2);
  etSumEtThresh.push_back(conf["ET_towerThreshold"].getValue<int>()/2);

  for (uint i=0; i<5; ++i) {
    paramsHelper.setEtSumEtaMax(i, etSumEtaMax.at(i));
    paramsHelper.setEtSumEtThreshold(i, etSumEtThresh.at(i));
  }

  paramsHelper.setJetCalibrationLUT ( l1t::convertToLUT( conf["jetEnergyCalibLUT"].getVector<uint32_t>() ) );
  paramsHelper.setEtSumEttCalibrationLUT    ( l1t::convertToLUT( conf["ET_energyCalibLUT"].getVector<int>() ) );
  paramsHelper.setEtSumEcalSumCalibrationLUT( l1t::convertToLUT( conf["ecalET_energyCalibLUT"].getVector<int>() ) );
  paramsHelper.setEtSumXCalibrationLUT      ( l1t::convertToLUT( conf["METX_energyCalibLUT"].getVector<int>() ) );
  paramsHelper.setEgMaxPtHOverE((conf["egammaRelaxationThreshold"].getValue<int>())/2.);
  paramsHelper.setEgEtaCut((conf["egammaMaxEta"].getValue<int>()));
  paramsHelper.setEgCalibrationLUT  ( l1t::convertToLUT( conf["egammaEnergyCalibLUT"].getVector<int>() ) );
  paramsHelper.setEgIsolationLUT    ( l1t::convertToLUT( conf["egammaIsoLUT"].getVector<int>() ) );

  paramsHelper.setIsoTauEtaMax((conf["tauMaxEta"].getValue<int>()));

  paramsHelper.setTauCalibrationLUT( l1t::convertToLUT( conf["tauEnergyCalibLUT"].getVector<int>() ) );
  paramsHelper.setTauIsolationLUT  ( l1t::convertToLUT( conf["tauIsoLUT1"].getVector<int>() ) );
  paramsHelper.setTauIsolationLUT2 ( l1t::convertToLUT( conf["tauIsoLUT2"].getVector<int>() ) );

  return true;
}

L1TCaloParamsOnlineProd::L1TCaloParamsOnlineProd(const edm::ParameterSet& iConfig) : L1ConfigOnlineProdBaseExt<L1TCaloParamsO2ORcd,l1t::CaloParams>(iConfig) {}

std::shared_ptr<l1t::CaloParams> L1TCaloParamsOnlineProd::newObject(const std::string& objectKey, const L1TCaloParamsO2ORcd& record) {
    using namespace edm::es;

    const L1TCaloStage2ParamsRcd& baseRcd = record.template getRecord< L1TCaloStage2ParamsRcd >() ;
    edm::ESHandle< l1t::CaloParams > baseSettings ;
    baseRcd.get( baseSettings ) ;


    if( objectKey.empty() ){
        edm::LogError( "L1-O2O: L1TCaloParamsOnlineProd" ) << "Key is empty, returning empty l1t::CaloParams";
        throw std::runtime_error("Empty objectKey");
    }

    std::string tscKey = objectKey.substr(0, objectKey.find(":") );
    std::string  rsKey = objectKey.substr(   objectKey.find(":")+1, std::string::npos );

    edm::LogInfo( "L1-O2O: L1TCaloParamsOnlineProd" ) << "Producing L1TCaloParamsOnlineProd with TSC key = " << tscKey << " and RS key = " << rsKey ;

    std::string calol1_top_key, calol1_algo_key, calol1_hw_key;
    std::string calol1_hw_payload, calol1_algo_payload;
    std::string calol2_top_key, calol2_algo_key, calol2_hw_key;
    std::string calol2_hw_payload;
    std::map<std::string,std::string> calol2_algo_payloads;  // key -> XML payload
    try {
        std::map<std::string,std::string> topKeys =
            l1t::OnlineDBqueryHelper::fetch( {"CALOL1_KEY","CALOL2_KEY"},
                                             "L1_TRG_CONF_KEYS",
                                             tscKey,
                                             m_omdsReader
                                           );
        calol1_top_key = topKeys["CALOL1_KEY"];

        std::map<std::string,std::string> calol1_keys =
            l1t::OnlineDBqueryHelper::fetch( {"ALGO","HW"},
                                             "CALOL1_KEYS",
                                             calol1_top_key,
                                             m_omdsReader
                                           );

        calol1_hw_key = calol1_keys["HW"];
        calol1_hw_payload = l1t::OnlineDBqueryHelper::fetch( {"CONF"},
                                                             "CALOL1_CLOBS",
                                                              calol1_hw_key,
                                                              m_omdsReader
                                                           ) ["CONF"];

        calol1_algo_key = calol1_keys["ALGO"];

        calol1_algo_payload = l1t::OnlineDBqueryHelper::fetch( {"CONF"},
                                                               "CALOL1_CLOBS",
                                                                calol1_algo_key,
                                                                m_omdsReader
                                                             ) ["CONF"];

        calol2_top_key = topKeys["CALOL2_KEY"];

        std::map<std::string,std::string> calol2_keys =
            l1t::OnlineDBqueryHelper::fetch( {"ALGO","HW"},
                                             "CALOL2_KEYS",
                                             calol2_top_key,
                                             m_omdsReader
                                           );

        calol2_hw_key = calol2_keys["HW"];
        calol2_hw_payload = l1t::OnlineDBqueryHelper::fetch( {"CONF"},
                                                             "CALOL2_CLOBS",
                                                              calol2_hw_key,
                                                              m_omdsReader
                                                           ) ["CONF"];

        calol2_algo_key = calol2_keys["ALGO"];

        std::map<std::string,std::string> calol2_algo_keys =
            l1t::OnlineDBqueryHelper::fetch( {"DEMUX","MPS_COMMON","MPS_JET","MP_EGAMMA","MP_SUM","MP_TAU"},
                                             "CALOL2_ALGO_KEYS",
                                             calol2_algo_key,
                                             m_omdsReader
                                           );

        for(auto &key : calol2_algo_keys)
            calol2_algo_payloads[ key.second ] = 
                l1t::OnlineDBqueryHelper::fetch( {"CONF"},
                                                 "CALOL2_CLOBS",
                                                 key.second,
                                                 m_omdsReader
                                               ) ["CONF"];

    } catch ( std::runtime_error &e ) {
        edm::LogError( "L1-O2O: L1TCaloParamsOnlineProd" ) << e.what();
        throw std::runtime_error("Broken key");
    }

    // for debugging purposes dump the configs to local files
    for(auto &conf : calol2_algo_payloads){ 
        std::ofstream output(std::string("/tmp/").append(conf.first.substr(0,conf.first.find("/"))).append(".xml"));
        output<<conf.second;
        output.close();
    }
    { 
        std::ofstream output(std::string("/tmp/").append(calol2_hw_key.substr(0,calol2_hw_key.find("/"))).append(".xml"));
        output << calol2_hw_payload;
        output.close();
    }
    { 
        std::ofstream output(std::string("/tmp/").append(calol1_algo_key.substr(0,calol1_algo_key.find("/"))).append(".xml"));
        output << calol1_algo_payload;
        output.close();
    }


    l1t::XmlConfigParser xmlReader1;
    l1t::TriggerSystem calol1;

if( true ){
    xmlReader1.readDOMFromString( calol1_hw_payload );
    xmlReader1.readRootElement  ( calol1, "calol1"  );
} else {
    // explicitly make the parser aware of the processors
    calol1.addProcessor("CTP7_Phi0", "Layer1Processor","-2","-0");
    calol1.addProcessor("CTP7_Phi1", "Layer1Processor","-2","-1");
    calol1.addProcessor("CTP7_Phi2", "Layer1Processor","-2","-2");
    calol1.addProcessor("CTP7_Phi3", "Layer1Processor","-2","-3");
    calol1.addProcessor("CTP7_Phi4", "Layer1Processor","-2","-4");
    calol1.addProcessor("CTP7_Phi5", "Layer1Processor","-2","-5");
    calol1.addProcessor("CTP7_Phi6", "Layer1Processor","-2","-6");
    calol1.addProcessor("CTP7_Phi7", "Layer1Processor","-2","-7");
    calol1.addProcessor("CTP7_Phi8", "Layer1Processor","-2","-8");
    calol1.addProcessor("CTP7_Phi9", "Layer1Processor","-2","-9");
    calol1.addProcessor("CTP7_Phi10","Layer1Processor","-2","-10");
    calol1.addProcessor("CTP7_Phi11","Layer1Processor","-2","-11");
    calol1.addProcessor("CTP7_Phi12","Layer1Processor","-2","-12");
    calol1.addProcessor("CTP7_Phi13","Layer1Processor","-2","-13");
    calol1.addProcessor("CTP7_Phi14","Layer1Processor","-2","-14");
    calol1.addProcessor("CTP7_Phi15","Layer1Processor","-2","-15");
    calol1.addProcessor("CTP7_Phi16","Layer1Processor","-2","-16");
}
    calol1.addProcessor("defaultProc", "processors","-2","0");

    //// the block of lines below allows to manage broken CaloL1 configurations
    calol1.addProcessor("processor0", "processors","-1","-1");
    calol1.addProcessor("processor1", "processors","-1","-2");
    calol1.addProcessor("processor2", "processors","-1","-3");
    calol1.addProcessor("processor3", "processors","-1","-4");
    calol1.addProcessor("processor4", "processors","-1","-5");
    calol1.addProcessor("processor5", "processors","-1","-6");
    calol1.addProcessor("processor6", "processors","-1","-7");
    calol1.addProcessor("processor7", "processors","-1","-8");
    calol1.addProcessor("processor8", "processors","-1","-9");
    calol1.addProcessor("processor9", "processors","-1","-10");
    calol1.addProcessor("processor10", "processors","-1","-11");
    calol1.addProcessor("processor11", "processors","-1","-12");
    calol1.addProcessor("processor12", "processors","-1","-13");
    calol1.addProcessor("processor13", "processors","-1","-14");
    calol1.addProcessor("processor14", "processors","-1","-15");
    calol1.addProcessor("processor15", "processors","-1","-16");
    calol1.addProcessor("processor16", "processors","-1","-17");
    calol1.addProcessor("processor17", "processors","-1","-18");
    ////

    xmlReader1.readDOMFromString( calol1_algo_payload );
    xmlReader1.readRootElement( calol1, "calol1" );

    calol1.setConfigured();

    std::map<std::string, l1t::Parameter> calol1_conf = calol1.getParameters("defaultProc");
    std::map<std::string, l1t::Mask>      calol1_rs   ;//= calol1.getMasks   ("processors");

    l1t::TriggerSystem calol2;

    l1t::XmlConfigParser xmlReader2;
    xmlReader2.readDOMFromString( calol2_hw_payload );
    xmlReader2.readRootElement( calol2, "calol2" );

    for(auto &conf : calol2_algo_payloads){ 
        xmlReader2.readDOMFromString( conf.second );
        xmlReader2.readRootElement( calol2, "calol2" );
    }

//    calol2.setSystemId("calol2");
    calol2.setConfigured();

    // Perhaps layer 2 has to look at settings for demux and mp separately? // => No demux settings required
    std::map<std::string, l1t::Parameter> calol2_conf = calol2.getParameters("MP1");
    std::map<std::string, l1t::Mask>      calol2_rs   ;//= calol2.getMasks   ("processors");
    
    l1t::CaloParamsHelper m_params_helper( *(baseSettings.product()) );

    if( !readCaloLayer1OnlineSettings(m_params_helper, calol1_conf, calol1_rs) )
        throw std::runtime_error("Parsing error for CaloLayer1");
    if( !readCaloLayer2OnlineSettings(m_params_helper, calol2_conf, calol2_rs) )
        throw std::runtime_error("Parsing error for CaloLayer2");

    std::shared_ptr< l1t::CaloParams > retval = std::make_shared< l1t::CaloParams >( m_params_helper ) ;
    return retval;
}

//define this as a plug-in
DEFINE_FWK_EVENTSETUP_MODULE(L1TCaloParamsOnlineProd);

