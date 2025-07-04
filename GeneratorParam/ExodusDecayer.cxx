// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#include "ExodusDecayer.h"
#include <TRandom.h>


ClassImp(ExodusDecayer)


ExodusDecayer::ExodusDecayer():
    fEPMassPion(0),
    fEPMassEta(0),
    fEPMassEtaDalitz(0),
    fEPMassEtaPrime(0),
    fEPMassEtaPrime_toOmega(0),
    fEPMassRho(0),
    fEPMassOmega(0),
    fEPMassOmegaDalitz(0),
    fEPMassPhi(0),
    fEPMassPhiDalitz(0),
    fEPMassPhiDalitz_toPi0(0),
    fEPMassJPsi(0),
    fEPMassPsi2S(0),
    fEPMassUpsilon(0),
    fPol(new TF1("dsigdcostheta","1.+[0]*x*x",-1.,1.)), /* Polarization Function for resonances */
    fInit(0),
    fDecayToDimuon(0)

{
// Constructor
}

ExodusDecayer::~ExodusDecayer()
{
  delete fEPMassPion;          
  delete fEPMassEta;       
  delete fEPMassEtaDalitz;       
  delete fEPMassEtaPrime;
  delete fEPMassEtaPrime_toOmega;
  delete fEPMassRho;
  delete fEPMassOmega;
  delete fEPMassOmegaDalitz;
  delete fEPMassPhi;
  delete fEPMassPhiDalitz;
  delete fEPMassPhiDalitz_toPi0;
  delete fEPMassJPsi;
  delete fEPMassPsi2S;
  delete fEPMassUpsilon;
  delete fPol;
}


void ExodusDecayer::Init()
{
 
// Initialisation
//
   Int_t ibin, nbins;
   Double_t min, maxpion, maxeta, maxomega, maxetaprime, maxetaprime_toOmega, maxphi, maxphi_toPi0, binwidth_pion, binwidth_eta, binwidth_omega, binwidth_etaprime, binwidth_etaprime_toOmega, binwidth_phi, binwidth_phi_toPi0;
   Double_t pionmass, etamass, omegamass, etaprimemass, phimass, emass, proton_mass, omasspion, omasseta, omassgamma;
   Double_t epsilon_pion, epsilon_eta, epsilon_omega, epsilon_etaprime, epsilon_etaprime_toOmega, epsilon_phi, epsilon_phi_toPi0;
   Double_t delta_pion, delta_eta, delta_omega, delta_etaprime, delta_etaprime_toOmega, delta_phi, delta_phi_toPi0;
   Double_t mLL_pion, mLL_eta, mLL_omega, mLL_etaprime, mLL_etaprime_toOmega, mLL_phi, mLL_phi_toPi0;
   Double_t q_pion, q_eta, q_omega, q_etaprime, q_etaprime_toOmega, q_phi, q_phi_toPi0;
   Double_t kwHelp_pion, kwHelp_eta, kwHelp_omega, kwHelp_etaprime, kwHelp_etaprime_toOmega, kwHelp_phi, kwHelp_phi_toPi0;
   Double_t krollWada_pion, krollWada_eta, krollWada_omega, krollWada_etaprime, krollWada_etaprime_toOmega, krollWada_phi, krollWada_phi_toPi0;
   Double_t formFactor_pion, formFactor_eta, formFactor_omega, formFactor_etaprime, formFactor_etaprime_toOmega, formFactor_phi, formFactor_phi_toPi0;
   Double_t weight_pion, weight_eta_dalitz, weight_omega_dalitz, weight_etaprime, weight_etaprime_toOmega, weight_phi_dalitz, weight_phi_dalitz_toPi0;

   Float_t  binwidth;
   Float_t  mass_bin, mass_min, mass_max;
   Double_t vmass_eta, vmass_rho, vmass_omega, vmass_phi, vmass_jpsi, vmass_psi2s, vmass_upsilon, vwidth_eta, vwidth_rho, vwidth_omega, vwidth_phi, vwidth_jpsi, vwidth_psi2s, vwidth_upsilon;
   Double_t weight_eta, weight_rho, weight_omega, weight_phi, weight_jpsi, weight_psi2s, weight_upsilon;

//================================================================================//
//          Create electron pair mass histograms from dalitz decays               //
//================================================================================//

    // Get the particle masses
    // parent
    nbins = 2000;
    mass_min = 0.;
    mass_max = 0.;
    pionmass     = (TDatabasePDG::Instance()->GetParticle(111))->Mass();
    etamass      = (TDatabasePDG::Instance()->GetParticle(221))->Mass();  
    omegamass    = (TDatabasePDG::Instance()->GetParticle(223))->Mass();  
    etaprimemass = (TDatabasePDG::Instance()->GetParticle(331))->Mass();  
    phimass      = (TDatabasePDG::Instance()->GetParticle(333))->Mass();
    // child - electron
    if (fDecayToDimuon == 0) emass = (TDatabasePDG::Instance()->GetParticle(11))->Mass();
    else emass = (TDatabasePDG::Instance()->GetParticle(13))->Mass();
    // child - other : third childs from Dalitz decays   
    omasspion  = pionmass;
    omasseta   = etamass;
    omassgamma = 0.;
       
    min         = 2.0 * emass;
    maxpion     = pionmass - omassgamma;
    maxeta      = etamass - omassgamma;
    maxomega    = omegamass - pionmass;
    maxetaprime = etaprimemass - omassgamma;
    maxetaprime_toOmega = etaprimemass - omegamass;
    if (fDecayToDimuon == 0) maxphi = phimass - omasseta; 
    else maxphi = phimass - omassgamma;
    maxphi_toPi0      = phimass - pionmass; 

    binwidth_pion     = (maxpion - min) / (Double_t)nbins;
    binwidth_eta      = (maxeta - min) / (Double_t)nbins;
    binwidth_omega    = (maxomega - min) / (Double_t)nbins;
    binwidth_etaprime = (maxetaprime - min) / (Double_t)nbins;
    binwidth_etaprime_toOmega = (maxetaprime_toOmega - min) / (Double_t)nbins;
    binwidth_phi      = (maxphi - min) / (Double_t)nbins;
    binwidth_phi_toPi0      = (maxphi_toPi0 - min) / (Double_t)nbins;


    epsilon_pion     = (emass / pionmass) * (emass / pionmass);
    epsilon_eta      = (emass / etamass) * (emass / etamass);
    epsilon_omega    = (emass / omegamass) * (emass / omegamass);
    epsilon_etaprime = (emass / etaprimemass) * (emass / etaprimemass);
    epsilon_etaprime_toOmega = (emass / etaprimemass) * (emass / etaprimemass);
    epsilon_phi      = (emass / phimass) * (emass / phimass);    
    epsilon_phi_toPi0      = (emass / phimass) * (emass / phimass);    


    delta_pion       = (omassgamma / pionmass) * (omassgamma / pionmass);
    delta_eta        = (omassgamma / etamass) * (omassgamma / etamass);
    delta_omega      = (omasspion / omegamass) * (omasspion / omegamass);
    delta_etaprime   = (omassgamma / etaprimemass) * (omassgamma / etaprimemass);
    delta_etaprime_toOmega   = (omegamass / etaprimemass) * (omegamass / etaprimemass);
    if (fDecayToDimuon == 0) delta_phi = (omasseta / phimass) * (omasseta / phimass);    
    else delta_phi = (omassgamma / phimass) * (omassgamma / phimass);    
    delta_phi_toPi0        = (pionmass / phimass) * (pionmass / phimass);    

    // create pair mass histograms for Dalitz decays of pi0, eta, omega, eta' and phi
    fEPMassPion = new TH1F("fEPMassPion", "Dalitz electron pair from pion", nbins, min, maxpion);
    fEPMassPion->SetDirectory(0);
    fEPMassEtaDalitz = new TH1F("fEPMassEtaDalitz", "Dalitz electron pair from eta", nbins, min, maxeta);
    fEPMassEtaDalitz->SetDirectory(0);
    fEPMassOmegaDalitz = new TH1F("fEPMassOmegaDalitz", "Dalitz electron pair from omega ", nbins, min, maxomega);
    fEPMassOmegaDalitz->SetDirectory(0);
    fEPMassEtaPrime = new TH1F("fEPMassEtaPrime", "Dalitz electron pair from etaprime", nbins, min, maxetaprime);
    fEPMassEtaPrime->SetDirectory(0);
    fEPMassEtaPrime_toOmega = new TH1F("fEPMassEtaPrime_toOmega", "Dalitz electron pair from etaprime_toOmega", nbins, min, maxetaprime_toOmega);
    fEPMassEtaPrime_toOmega->SetDirectory(0);
    fEPMassPhiDalitz = new TH1F("fEPMassPhiDalitz", "Dalitz electron pair from phi ", nbins, min, maxphi);
    fEPMassPhiDalitz->SetDirectory(0);
    fEPMassPhiDalitz_toPi0  = new TH1F("fEPMassPhiDalitz_toPi0", "Dalitz electron pair from phi_toPi0 ", nbins, min, maxphi_toPi0);
    fEPMassPhiDalitz_toPi0->SetDirectory(0);

    mLL_pion =  mLL_eta = mLL_omega = mLL_etaprime = mLL_etaprime_toOmega = mLL_phi = mLL_phi_toPi0 = 0.;

    for (ibin = 1; ibin <= nbins; ibin++ )
        {
         mLL_pion     = min + (Double_t)(ibin - 1) * binwidth_pion + binwidth_pion / 2.0;
         mLL_eta      = min + (Double_t)(ibin - 1) * binwidth_eta + binwidth_eta / 2.0;
         mLL_omega    = min + (Double_t)(ibin - 1) * binwidth_omega + binwidth_omega / 2.0;
         mLL_etaprime = min + (Double_t)(ibin - 1) * binwidth_etaprime + binwidth_etaprime / 2.0;
         mLL_etaprime_toOmega = min + (Double_t)(ibin - 1) * binwidth_etaprime_toOmega + binwidth_etaprime_toOmega / 2.0;
         mLL_phi      = min + (Double_t)(ibin - 1) * binwidth_phi + binwidth_phi / 2.0;
         mLL_phi_toPi0      = min + (Double_t)(ibin - 1) * binwidth_phi_toPi0 + binwidth_phi_toPi0 / 2.0;


         q_pion        = (mLL_pion / pionmass) * (mLL_pion / pionmass);
         q_eta         = (mLL_eta / etamass) * (mLL_eta / etamass);
         q_omega       = (mLL_omega / omegamass)*(mLL_omega / omegamass);
         q_etaprime    = (mLL_etaprime / etaprimemass) * (mLL_etaprime / etaprimemass);
         q_etaprime_toOmega    = (mLL_etaprime_toOmega / etaprimemass) * (mLL_etaprime_toOmega / etaprimemass);
         q_phi         = (mLL_phi / phimass) * (mLL_phi / phimass);
         q_phi_toPi0         = (mLL_phi_toPi0 / phimass) * (mLL_phi_toPi0 / phimass);

    if (fDecayToDimuon == 0)
      {
      if ( q_pion <= 4.0 * epsilon_pion || q_eta <= 4.0 * epsilon_eta || q_omega <= 4.0 * epsilon_omega || q_etaprime <= 4.0 * epsilon_etaprime || q_etaprime_toOmega <= 4.0 * epsilon_etaprime_toOmega || q_phi <= 4.0 * epsilon_phi || q_phi_toPi0 <= 4.0 * epsilon_phi_toPi0  )
         {
           printf("ExodusDecayer: Error in calculating Dalitz mass histogram binning!\n");
         }
      } else {
      if ( q_eta <= 4.0 * epsilon_eta || q_omega <= 4.0 * epsilon_omega || q_etaprime <= 4.0 * epsilon_etaprime || q_phi <= 4.0 * epsilon_phi)
         {
           printf("ExodusDecayer: Error in calculating Dalitz mass histogram binning!\n");
         }
      }
  

    kwHelp_pion     = (1.0 + q_pion /  (1.0 - delta_pion)) * (1.0 + q_pion / (1.0 - delta_pion))
                                     - 4.0 * q_pion / ((1.0 - delta_pion) * (1.0 - delta_pion));

    kwHelp_eta      = (1.0 + q_eta /  (1.0 - delta_eta)) * (1.0 + q_eta / (1.0 - delta_eta))
                                    - 4.0 * q_eta / ((1.0 - delta_eta) * (1.0 - delta_eta));

    kwHelp_omega    = (1.0 + q_omega /  (1.0 - delta_omega)) * (1.0 + q_omega / (1.0 - delta_omega))
                                      - 4.0 * q_omega / ((1.0 - delta_omega) * (1.0 - delta_omega));

    kwHelp_etaprime = (1.0 + q_etaprime /  (1.0 - delta_etaprime)) * (1.0 + q_etaprime / (1.0 - delta_etaprime))
                                         - 4.0 * q_etaprime / ((1.0 - delta_etaprime) * (1.0 - delta_etaprime));
    kwHelp_etaprime_toOmega = (1.0 + q_etaprime_toOmega /  (1.0 - delta_etaprime_toOmega)) * (1.0 + q_etaprime_toOmega / (1.0 - delta_etaprime_toOmega))
                                    - 4.0 * q_etaprime_toOmega / ((1.0 - delta_etaprime_toOmega) * (1.0 - delta_etaprime_toOmega));
    kwHelp_phi      = (1.0 + q_phi /  (1.0 - delta_phi)) * (1.0 + q_phi / (1.0 - delta_phi))
                                    - 4.0 * q_phi / ((1.0 - delta_phi) * (1.0 - delta_phi));
    kwHelp_phi_toPi0      = (1.0 + q_phi_toPi0 /  (1.0 - delta_phi_toPi0)) * (1.0 + q_phi_toPi0 / (1.0 - delta_phi_toPi0))
                                    - 4.0 * q_phi_toPi0 / ((1.0 - delta_phi_toPi0) * (1.0 - delta_phi_toPi0));




    if (fDecayToDimuon == 0)
      {
      if ( kwHelp_pion <= 0.0 || kwHelp_eta <= 0.0 || kwHelp_omega <= 0.0 || kwHelp_etaprime <= 0.0 || kwHelp_etaprime_toOmega <= 0.0 || kwHelp_phi <= 0.0 || kwHelp_phi_toPi0 <= 0.0 )
         {
           printf("ExodusDecayer: Error in calculating Dalitz mass histogram binning!\n");
         }
      } else {
      if ( kwHelp_eta <= 0.0 || kwHelp_omega <= 0.0 || kwHelp_etaprime <= 0.0 || kwHelp_phi <= 0.0)
         {
           printf("ExodusDecayer: Error in calculating Dalitz mass histogram binning!\n");
         }
      }


 // Invariant mass distributions of electron pairs from Dalitz decays
 // using Kroll-Wada function   

      krollWada_pion     = (2.0 / mLL_pion) * TMath::Exp(1.5 * TMath::Log(kwHelp_pion))
                                            * TMath::Sqrt(1.0 - 4.0 * epsilon_pion / q_pion)
                                            * (1.0 + 2.0 * epsilon_pion / q_pion);
    
     
      krollWada_eta      = (2.0 / mLL_eta) * TMath::Exp(1.5 * TMath::Log(kwHelp_eta))
                                           * TMath::Sqrt(1.0 - 4.0 * epsilon_eta / q_eta)
                                           * (1.0 + 2.0 * epsilon_eta / q_eta);
   
   
      krollWada_omega    = (2.0 / mLL_omega) * TMath::Exp(1.5 * TMath::Log(kwHelp_omega))
                                             * TMath::Sqrt(1.0 - 4.0 * epsilon_omega / q_omega)
                                             * (1.0 + 2.0 * epsilon_omega / q_omega);
   
   
      krollWada_etaprime = (2.0 / mLL_etaprime) * TMath::Exp(1.5 * TMath::Log(kwHelp_etaprime))
                                                * TMath::Sqrt(1.0 - 4.0 * epsilon_etaprime / q_etaprime)
                                                * (1.0 + 2.0 * epsilon_etaprime / q_etaprime);
      krollWada_etaprime_toOmega = (2.0 / mLL_etaprime_toOmega) * TMath::Exp(1.5 * TMath::Log(kwHelp_etaprime_toOmega))
                                                * TMath::Sqrt(1.0 - 4.0 * epsilon_etaprime_toOmega / q_etaprime_toOmega)
                                                * (1.0 + 2.0 * epsilon_etaprime_toOmega / q_etaprime_toOmega);

  
 
      krollWada_phi      = (2.0 / mLL_phi) * TMath::Exp(1.5 * TMath::Log(kwHelp_phi))
                                           * TMath::Sqrt(1.0 - 4.0 * epsilon_phi / q_phi)
                                           * (1.0 + 2.0 * epsilon_phi / q_phi);   



      krollWada_phi_toPi0      = (2.0 / mLL_phi_toPi0) * TMath::Exp(1.5 * TMath::Log(kwHelp_phi_toPi0))
                                           * TMath::Sqrt(1.0 - 4.0 * epsilon_phi_toPi0 / q_phi_toPi0)
                                           * (1.0 + 2.0 * epsilon_phi_toPi0 / q_phi_toPi0);


    // Form factors from Lepton-G  (etaprime is a private B-W fit to Lepton-G data)
    formFactor_pion     = 1.0/(1.0-5.5*mLL_pion*mLL_pion);
    formFactor_etaprime = (TMath::Power(TMath::Power(0.764,2),2))
                          /(TMath::Power(TMath::Power(0.764,2)-TMath::Power(mLL_etaprime, 2), 2)
                          + TMath::Power(0.1020, 2)*TMath::Power(0.764, 2));
    formFactor_etaprime_toOmega = (TMath::Power(TMath::Power(0.764,2),2))
                          /(TMath::Power(TMath::Power(0.764,2)-TMath::Power(mLL_etaprime_toOmega, 2), 2)
                          + TMath::Power(0.1020, 2)*TMath::Power(0.764, 2));
    // Form factors from  NA60 (omega is a private B-W fit to NA60 data)
    formFactor_eta      = 1.0/(1.0-1.934*mLL_eta*mLL_eta);
    formFactor_omega    = (TMath::Power(TMath::Power(0.67070,2),2))
                          /(TMath::Power(TMath::Power(0.67070,2)-TMath::Power(mLL_omega, 2), 2)
                          + TMath::Power(0.0534321, 2)*TMath::Power(0.67070, 2));
    // No form factor for phi:
    formFactor_phi      = 1.0; 
    formFactor_phi_toPi0      = 1.0; 




    weight_pion         = krollWada_pion * formFactor_pion * formFactor_pion;
    weight_eta_dalitz   = krollWada_eta * formFactor_eta * formFactor_eta;
    weight_omega_dalitz = krollWada_omega * formFactor_omega;
    weight_etaprime     = krollWada_etaprime * formFactor_etaprime;
    weight_etaprime_toOmega     = krollWada_etaprime_toOmega * formFactor_etaprime_toOmega;
    weight_phi_dalitz   = krollWada_phi * formFactor_phi * formFactor_phi;
    weight_phi_dalitz_toPi0   = krollWada_phi_toPi0 * formFactor_phi_toPi0 * formFactor_phi_toPi0;


    // Fill histograms of electron pair masses from dalitz decays
    fEPMassPion       ->AddBinContent(ibin, weight_pion);
    fEPMassEtaDalitz  ->AddBinContent(ibin, weight_eta_dalitz);
    fEPMassOmegaDalitz->AddBinContent(ibin, weight_omega_dalitz);
    fEPMassEtaPrime   ->AddBinContent(ibin, weight_etaprime);
    fEPMassEtaPrime_toOmega   ->AddBinContent(ibin, weight_etaprime_toOmega);
    fEPMassPhiDalitz  ->AddBinContent(ibin, weight_phi_dalitz);
    fEPMassPhiDalitz_toPi0  ->AddBinContent(ibin, weight_phi_dalitz_toPi0);
    }


   

//===================================================================================//
//         Create electron pair mass histograms from resonance decays                //
//===================================================================================//

   Double_t pimass = 0.13956995;

   // Get the particle masses
   // parent
   vmass_eta   = (TDatabasePDG::Instance()->GetParticle(221))->Mass();  
   vmass_rho   = (TDatabasePDG::Instance()->GetParticle(113))->Mass();  
   vmass_omega = (TDatabasePDG::Instance()->GetParticle(223))->Mass();  
   vmass_phi   = (TDatabasePDG::Instance()->GetParticle(333))->Mass();  
   vmass_jpsi  = (TDatabasePDG::Instance()->GetParticle(443))->Mass();
   vmass_psi2s  = (TDatabasePDG::Instance()->GetParticle(100443))->Mass();
   vmass_upsilon  = (TDatabasePDG::Instance()->GetParticle(553))->Mass();
   // Get the particle widths
   // parent
   vwidth_eta   = (TDatabasePDG::Instance()->GetParticle(221))->Width();  
   vwidth_rho   = (TDatabasePDG::Instance()->GetParticle(113))->Width();   
   vwidth_omega = (TDatabasePDG::Instance()->GetParticle(223))->Width();  
   vwidth_phi   = (TDatabasePDG::Instance()->GetParticle(333))->Width();  
   vwidth_jpsi  = (TDatabasePDG::Instance()->GetParticle(443))->Width();
   vwidth_psi2s  = (TDatabasePDG::Instance()->GetParticle(100443))->Width();
   vwidth_upsilon  = (TDatabasePDG::Instance()->GetParticle(553))->Width();

   if( vwidth_jpsi < 1e-6 ){
    printf(Form("ExodusDecayer: Warning: Width JPsi = %f (Mass = %f) < 1e-6, set to 1e-6\n",vwidth_jpsi,vmass_jpsi));
    vwidth_jpsi = 1e-6;
  }
  if( vwidth_psi2s < 1e-6 ){
    printf(Form("ExodusDecayer: Warning: Width Psi2S = %f (Mass = %f) < 1e-6, set to 1e-6\n",vwidth_psi2s,vmass_psi2s));
    vwidth_psi2s = 1e-6;
  }
  if( vwidth_upsilon < 1e-6 ){
    printf(Form("ExodusDecayer: Warning: Width Upsilon = %f (Mass = %f) < 1e-6, set to 1e-6\n",vwidth_upsilon,vmass_upsilon));
    vwidth_upsilon = 1e-6;
  }


       if ( mass_min == 0. && mass_max == 0. )
          {
           mass_min  = 2.*pimass;
           mass_max  = 10.;
          }

     binwidth  = (mass_max-mass_min)/(Double_t)nbins;

     // create pair mass histograms for resonances of rho, omega, phi, jpsi, psi2s and upsilon
     fEPMassEta    = new TH1F("fEPMassEta","mass eta",nbins,mass_min,mass_max);
     fEPMassEta->SetDirectory(0);
     fEPMassRho    = new TH1F("fEPMassRho","mass rho",nbins,mass_min,mass_max);
     fEPMassRho->SetDirectory(0);
     fEPMassOmega  = new TH1F("fEPMassOmega","mass omega",nbins,mass_min,mass_max);
     fEPMassOmega->SetDirectory(0);
     fEPMassPhi    = new TH1F("fEPMassPhi","mass phi",nbins,mass_min,mass_max);
     fEPMassPhi->SetDirectory(0);
     fEPMassJPsi   = new TH1F("fEPMassJPsi","mass jpsi",nbins,mass_min,mass_max);
     fEPMassJPsi->SetDirectory(0);
     fEPMassPsi2S   = new TH1F("fEPMassPsi2S","mass psi2s",nbins,mass_min,mass_max);
     fEPMassPsi2S->SetDirectory(0);
     fEPMassUpsilon   = new TH1F("fEPMassUpsilon","mass upsilon",nbins,mass_min,mass_max);
     fEPMassUpsilon->SetDirectory(0);

     for (ibin=1; ibin<=nbins; ibin++ )
     {
     mass_bin = mass_min+(Double_t)(ibin-1)*binwidth+binwidth/2.0;

//     weight_rho     = (Float_t)GounarisSakurai(mass_bin,vmass_rho,vwidth_rho,emass);
     weight_eta     = (Float_t)Lorentz(mass_bin,vmass_eta,vwidth_eta);
     weight_rho     = (Float_t)RhoShapeFromNA60(mass_bin,vmass_rho,vwidth_rho,emass);

     weight_omega   = (Float_t)GounarisSakurai(mass_bin,vmass_omega,vwidth_omega,emass);
     weight_phi     = (Float_t)GounarisSakurai(mass_bin,vmass_phi,vwidth_phi,emass); 
     weight_jpsi    = (Float_t)Lorentz(mass_bin,vmass_jpsi,vwidth_jpsi);
     weight_psi2s    = (Float_t)Lorentz(mass_bin,vmass_psi2s,vwidth_psi2s);
     weight_upsilon    = (Float_t)Lorentz(mass_bin,vmass_upsilon,vwidth_upsilon);

     // Fill histograms of electron pair masses from resonance decays
     fEPMassEta  ->AddBinContent(ibin,weight_eta);
     fEPMassRho  ->AddBinContent(ibin,weight_rho);
     fEPMassOmega->AddBinContent(ibin,weight_omega);
     fEPMassPhi  ->AddBinContent(ibin,weight_phi);
     fEPMassJPsi ->AddBinContent(ibin,weight_jpsi);
     fEPMassPsi2S ->AddBinContent(ibin,weight_psi2s);
     fEPMassUpsilon ->AddBinContent(ibin,weight_upsilon);
    }  
    fInit=1;
}


Double_t ExodusDecayer::GounarisSakurai(Float_t mass, Double_t vmass, Double_t vwidth, Double_t emass)
{
// Invariant mass distributions of electron pairs from resonance decays
// of rho (not anymore, now using the shape from NA60), omega and phi
// using Gounaris-Sakurai function

  Double_t corr = 0.;
  Double_t epsilon = 0.;
  Double_t weight = 0.;

  Double_t pimass = 0.13956995;
 
  if(mass>pimass){
  corr = vwidth*(vmass/mass)*exp(1.5*log((mass*mass/4.0-pimass*pimass)
         /(vmass*vmass/4.0-pimass*pimass)));
  }

  epsilon = (emass/mass)*(emass/mass);
       
  if ( 1.0-4.0*epsilon>=0.0 )
  {
   weight = sqrt(1.0-4.0*epsilon)*(1.0+2.0*epsilon)/
                ((vmass*vmass-mass*mass)*(vmass*vmass-mass*mass)+
                (vmass*corr)*(vmass*corr));
  }
  return weight;  
}




Double_t ExodusDecayer::RhoShapeFromNA60(Float_t mass, Double_t vmass, Double_t vwidth, Double_t emass)
{
  // Invariant mass distributions of electron pairs from rho decay
  // Using NA06 shape and measured temperature parameter from Physics Letters B 757 (2016) 437–444
  // Parameterization implemented by Hiroyuki Sako (GSI)

  double r, GammaTot;
  Double_t mRho    = vmass;
  Double_t Gamma0  = vwidth;
  Double_t mPi     = TDatabasePDG::Instance()->GetParticle("pi0")->Mass();

  //HERE hack: substitute original muon mass for electron mass:
  //Double_t mMu     = TDatabasePDG::Instance()->GetParticle("mu-")->Mass();
  Double_t mMu     = TDatabasePDG::Instance()->GetParticle("e-")->Mass();
  if (fDecayToDimuon == 1) mMu = TDatabasePDG::Instance()->GetParticle("mu-")->Mass();

  const double Norm = 0.0744416*1.01;
  // 0.0744416 at m = 0.72297
  // is the max number with Norm=1 (for rho)

  double mThreshold = 2.*mPi;

  //  const double T = 0.170; // Assumption of pi+ temperature [GeV/c2]
  const double T=0.161;//measured value

  if (mass < mThreshold) {
    r = 0.;
    return r;
  }

  double k = sqrt(0.25*mass*mass-(mThreshold/2)*(mThreshold/2));
  double k0 = sqrt(0.25*mRho*mRho-(mThreshold/2)*(mThreshold/2));

  GammaTot = (k/k0)*(k/k0)*(k/k0)*(mRho/mass)*(mRho/mass)*Gamma0;

  double FormFactor2 = 1/((mass*mass-mRho*mRho)*(mass*mass-mRho*mRho)+
              mass*mass*GammaTot*GammaTot);

  r = pow(mass,1.5)*pow((1-mThreshold*mThreshold/(mass*mass)),1.5)*
((mass*mass+2*mMu*mMu)/(mass*mass))*(pow((mass*mass-4*mMu*mMu),0.5)/mass)*FormFactor2
    *exp(-mass/T)/Norm;

  return r;
}





Double_t ExodusDecayer::Lorentz(Float_t mass, Double_t vmass, Double_t vwidth)
{
// Invariant mass distributions of electron pairs from resonance decay
// of jpsi (and it can also be used for other particles except rho, omega and phi) 
// using Lorentz function

  Double_t weight;

  weight = (vwidth*vwidth/4.0)/(vwidth*vwidth/4.0+(vmass-mass)*(vmass-mass));

  return weight;

}


void ExodusDecayer::Decay(Int_t idpart, TLorentzVector* pparent)
{
 
    if (!fInit) {
        Init();
        fInit=1;  
    }


   // -------- get id of the partner from idpart  ------- //
   Int_t idpartner=idpart/1000;
   if (idpart!=100443){ //protect psi(2S) pdg code (100443)
    idpart=idpart%1000;
   } else {
    idpartner=0;
   }

   //local variables for dalitz/2-body decay:
   Double_t pmass, epmass, realp_mass, e1, p1, e3, p3;
   Double_t wp_res, mp_res, md_res, epmass_res, Ed_res, pd_res;
   Double_t PolPar;
   TLorentzVector fProducts_res[2], fProducts_dalitz[3];
   Int_t idRho=113;
   Int_t idOmega=223;
   Int_t idPhi=333;
   Int_t idJPsi=443;
   Int_t idPi0=111;
   Int_t idEta=221;
   Int_t idEtaPrime=331;
   Int_t idPsi2S=100443;
   Int_t idUpsilon=553;

   // Get the particle masses of daughters
   Double_t emass, proton_mass, omass_pion, omass_eta, omass_gamma, omass_omega;
   if (fDecayToDimuon == 0) emass = (TDatabasePDG::Instance()->GetParticle(11))->Mass();
   else emass = (TDatabasePDG::Instance()->GetParticle(13))->Mass();
   proton_mass = (TDatabasePDG::Instance()->GetParticle(2212)) ->Mass();  
   omass_pion  = (TDatabasePDG::Instance()->GetParticle(111))->Mass();
   omass_eta   = (TDatabasePDG::Instance()->GetParticle(221))->Mass();  
   omass_gamma = (TDatabasePDG::Instance()->GetParticle(22)) ->Mass();   
   omass_omega = (TDatabasePDG::Instance()->GetParticle(223)) ->Mass();   

   //flat angular distributions
   Double_t costheta, sintheta, cosphi, sinphi, phi;
   Double_t beta_square, lambda;
   costheta = (2.0 * gRandom->Rndm()) - 1.;
   sintheta = TMath::Sqrt((1. + costheta) * (1. - costheta));
   phi      = 2.0 * TMath::ACos(-1.) * gRandom->Rndm();
   sinphi   = TMath::Sin(phi);
   cosphi   = TMath::Cos(phi); 


//-----------------------------------------------------------------------------//
//             Generate Dalitz decays: Pi0/Eta/Omega/EtaPrime/Phi              //
//-----------------------------------------------------------------------------//

  if((idpart==idPi0||idpart==idEta||idpart==idOmega||idpart==idEtaPrime||idpart==idPhi)&&(idpartner!=0)){

   //get the parent mass
   pmass = pparent->M();

   // Sample the electron pair mass from a histogram
   for(;;){
        if(idpart==idPi0){
         epmass = fEPMassPion->GetRandom();
         realp_mass=omass_gamma;
        }else if(idpart==idEta){
         epmass = fEPMassEtaDalitz->GetRandom();
         realp_mass=omass_gamma;
        }else if(idpart==idOmega){
         epmass = fEPMassOmegaDalitz->GetRandom();
         realp_mass=omass_pion;
        }else if(idpart==idEtaPrime){
         if(idpartner==22){
          epmass = fEPMassEtaPrime->GetRandom();
          realp_mass=omass_gamma;
         }else if (idpartner==223 && fDecayToDimuon == 0){
          epmass = fEPMassEtaPrime_toOmega->GetRandom();
          realp_mass=omass_omega;
         }
        }else if(idpart==idPhi){
         if(idpartner==221 && fDecayToDimuon == 0){
          epmass = fEPMassPhiDalitz->GetRandom();
          realp_mass=omass_eta;
         }else if (idpartner==111 && fDecayToDimuon == 0){
          epmass = fEPMassPhiDalitz_toPi0->GetRandom();
          realp_mass=omass_pion;
         }else if (idpartner==22 && fDecayToDimuon == 1){
          epmass = fEPMassPhiDalitz->GetRandom();
          realp_mass=omass_gamma;
         }

        }else{ printf("ExodusDecyer: ERROR: Dalitz mass parametrization not found \n");
               return;
        }
        if(pmass-realp_mass>epmass && epmass/2.>emass) break;
   }

   // electron pair kinematics in virtual photon rest frame
   e1 = epmass / 2.;
   p1 = TMath::Sqrt((e1 + emass) * (e1 - emass));


   //Polarization parameters (lambda) for Dalitz:
   if ( realp_mass<0.01 ){
    beta_square = 1.0 - 4.0*(emass*emass)/(epmass*epmass);
    lambda      = beta_square/(2.0-beta_square);
    do{
     costheta = (2.0*gRandom->Rndm())-1.;
    }
    while ( (1.0+lambda*costheta*costheta)<(2.0*gRandom->Rndm()) );
    sintheta = TMath::Sqrt((1. + costheta) * (1. - costheta));
    phi      = 2.0 * TMath::ACos(-1.) * gRandom->Rndm();
    sinphi   = TMath::Sin(phi);
    cosphi   = TMath::Cos(phi); 
   }

   // momentum vectors of electrons in virtual photon rest frame
   Double_t pProd1[3] = {p1 * sintheta * cosphi,
                         p1 * sintheta * sinphi,
                         p1 * costheta};
   Double_t pProd2[3] = {-1.0 * p1 * sintheta * cosphi,
                         -1.0 * p1 * sintheta * sinphi,
                         -1.0 * p1 * costheta};
   fProducts_dalitz[0].SetPx(pProd1[0]);
   fProducts_dalitz[0].SetPy(pProd1[1]);
   fProducts_dalitz[0].SetPz(pProd1[2]);
   fProducts_dalitz[0].SetE(e1);
   fProducts_dalitz[1].SetPx(pProd2[0]);
   fProducts_dalitz[1].SetPy(pProd2[1]);
   fProducts_dalitz[1].SetPz(pProd2[2]);
   fProducts_dalitz[1].SetE(e1);

   // third child kinematics in parent meson rest frame
   e3 = (pmass*pmass + realp_mass*realp_mass - epmass*epmass)/(2. * pmass);
   p3 = TMath::Sqrt((e3+realp_mass) * (e3-realp_mass));
   
   // third child 4-vector in parent meson rest frame
   costheta = (2.0 * gRandom->Rndm()) - 1.;
   sintheta = TMath::Sqrt((1. + costheta) * (1. - costheta));
   phi      = 2.0 * TMath::ACos(-1.) * gRandom->Rndm();
   sinphi   = TMath::Sin(phi);
   cosphi   = TMath::Cos(phi); 
   fProducts_dalitz[2].SetPx(p3 * sintheta * cosphi);
   fProducts_dalitz[2].SetPy(p3 * sintheta * sinphi);
   fProducts_dalitz[2].SetPz(p3 * costheta);
   fProducts_dalitz[2].SetE(e3);

   // boost the dielectron into the parent meson's rest frame
   Double_t eLPparent = TMath::Sqrt(p3*p3 + epmass*epmass);
   TVector3 boostPair( -1.0 * fProducts_dalitz[2].Px() / eLPparent,
                       -1.0 * fProducts_dalitz[2].Py() / eLPparent,
                       -1.0 * fProducts_dalitz[2].Pz() / eLPparent);
   fProducts_dalitz[0].Boost(boostPair);
   fProducts_dalitz[1].Boost(boostPair);

   // boost all decay products into the lab frame
   TVector3 boostLab(pparent->Px() / pparent->E(),
                     pparent->Py() / pparent->E(),
                     pparent->Pz() / pparent->E());
   fProducts_dalitz[0].Boost(boostLab);
   fProducts_dalitz[1].Boost(boostLab);
   fProducts_dalitz[2].Boost(boostLab);

   if(idpart==idPi0) {
     fProducts_pion[0]=fProducts_dalitz[0];
     fProducts_pion[1]=fProducts_dalitz[1];
     fProducts_pion[2]=fProducts_dalitz[2];
   }else if(idpart==idEta){
     fProducts_eta_dalitz[0]=fProducts_dalitz[0];
     fProducts_eta_dalitz[1]=fProducts_dalitz[1];
     fProducts_eta_dalitz[2]=fProducts_dalitz[2];
   }else if(idpart==idOmega){
     fProducts_omega_dalitz[0]=fProducts_dalitz[0];
     fProducts_omega_dalitz[1]=fProducts_dalitz[1];
     fProducts_omega_dalitz[2]=fProducts_dalitz[2];
   }else if(idpart==idEtaPrime){
     fProducts_etaprime[0]=fProducts_dalitz[0];
     fProducts_etaprime[1]=fProducts_dalitz[1];
     fProducts_etaprime[2]=fProducts_dalitz[2];
   }else if(idpart==idPhi){
     fProducts_phi_dalitz[0]=fProducts_dalitz[0];
     fProducts_phi_dalitz[1]=fProducts_dalitz[1];
     fProducts_phi_dalitz[2]=fProducts_dalitz[2];
   }

  }


//-----------------------------------------------------------------------------//
//   Generate 2-body resonance decays: Rho/Omega/Phi/JPsi/Psi2S/Upsilon        //
//-----------------------------------------------------------------------------//
  if(((idpart==idEta)||idpart==idRho||idpart==idOmega||idpart==idPhi||idpart==idJPsi||idpart==idPsi2S||idpart==idUpsilon)&&(idpartner==0)){

   //get the parent mass
   mp_res = pparent->M();

   //check daughters mass
   md_res=emass;
   if ( mp_res < 2.*md_res ){
        printf("ExodusDecayer: res into ee Decay kinematically impossible! \n");
        return;
   }
   // Sample the electron pair mass from a histogram and set Polarization
   for( ;; ) {
        if(idpart==idEta){
         epmass_res = fEPMassEta->GetRandom();
         PolPar=0.;
        }else if(idpart==idRho){
         epmass_res = fEPMassRho->GetRandom();
	 PolPar=0.;
        }else if(idpart==idOmega){
	 epmass_res = fEPMassOmega->GetRandom();
	 PolPar=0.;
        }else if(idpart==idPhi){
	 epmass_res = fEPMassPhi->GetRandom();
	 PolPar=0.;
        }else if(idpart==idPhi){
	 epmass_res = fEPMassPhi->GetRandom();
	 PolPar=0.;
        }else if(idpart==idJPsi){
	 epmass_res = fEPMassJPsi->GetRandom();
	 PolPar=0.;
        }else if(idpart==idPsi2S){
     epmass_res = fEPMassPsi2S->GetRandom();
     PolPar=0.;
        } else if(idpart==idUpsilon){
     epmass_res = fEPMassUpsilon->GetRandom();
     PolPar=0.;
        }else{ printf("ExodusDecayer: ERROR: Resonance mass G-S parametrization not found \n");
               return;
        }
        if ( mp_res < 2.*epmass_res ) break;
   }

   // electron pair kinematics in virtual photon rest frame
   Ed_res = epmass_res/2.;
   pd_res = TMath::Sqrt((Ed_res+md_res)*(Ed_res-md_res));

   // momentum vectors of electrons in virtual photon rest frame 
   fPol->SetParameter(0,PolPar);
   costheta = fPol->GetRandom();
   sintheta = TMath::Sqrt((1. + costheta)*(1. - costheta));
   fProducts_res[0].SetPx(pd_res * sintheta * cosphi);
   fProducts_res[0].SetPy(pd_res * sintheta * sinphi);
   fProducts_res[0].SetPz(pd_res * costheta);
   fProducts_res[0].SetE(Ed_res);
   fProducts_res[1].SetPx(-1.0 * pd_res * sintheta * cosphi);
   fProducts_res[1].SetPy(-1.0 * pd_res * sintheta * sinphi);
   fProducts_res[1].SetPz(-1.0 * pd_res * costheta);
   fProducts_res[1].SetE(Ed_res);

   // Beam parameters in LAB frame
   TLorentzVector pProj, pTarg; 
   Double_t BeamE=3500.;
   pProj.SetPxPyPzE(0.,0.,-1.*BeamE,TMath::Sqrt(BeamE*BeamE+proton_mass*proton_mass)); // Beam 1
   pTarg.SetPxPyPzE(0.,0.,BeamE,TMath::Sqrt(BeamE*BeamE+proton_mass*proton_mass)); // Beam 2

   //re-build parent with G-S mass
   TLorentzVector pparent_corr;
   pparent_corr.SetPx(pparent->Px());
   pparent_corr.SetPy(pparent->Py());
   pparent_corr.SetPz(pparent->Pz());
   pparent_corr.SetE(sqrt(pow(pparent->P(),2)+pow(epmass_res,2)));

   //Boost Beam from CM to Resonance rest frame
   TVector3 betaResCM;
   betaResCM = (-1./pparent_corr.E()*pparent_corr.Vect());
   pProj.Boost(betaResCM);   
   pTarg.Boost(betaResCM);

   //Define Zaxis in C-S frame and rotate legs to it
   TVector3 zaxisCS;
   zaxisCS=(((pProj.Vect()).Unit())-((pTarg.Vect()).Unit())).Unit();
   fProducts_res[0].RotateUz(zaxisCS);
   fProducts_res[1].RotateUz(zaxisCS);

   // boost decay products into the lab frame 
   TVector3 boostLab_res_corr(pparent_corr.Px() / pparent_corr.E(),
                         pparent_corr.Py() / pparent_corr.E(),
                         pparent_corr.Pz() / pparent_corr.E());
   fProducts_res[0].Boost(boostLab_res_corr);
   fProducts_res[1].Boost(boostLab_res_corr);

   if(idpart==idEta) {
    fProducts_eta[0]=fProducts_res[0];
    fProducts_eta[1]=fProducts_res[1];
   }else if(idpart==idRho) {
    fProducts_rho[0]=fProducts_res[0];
    fProducts_rho[1]=fProducts_res[1];
   }else if(idpart==idOmega){
    fProducts_omega[0]=fProducts_res[0];
    fProducts_omega[1]=fProducts_res[1];
   }else  if(idpart==idPhi){
    fProducts_phi[0]=fProducts_res[0];
    fProducts_phi[1]=fProducts_res[1];
   }else  if(idpart==idJPsi){
    fProducts_jpsi[0]=fProducts_res[0];
    fProducts_jpsi[1]=fProducts_res[1];
   }else  if(idpart==idPsi2S){
    fProducts_psi2s[0]=fProducts_res[0];
    fProducts_psi2s[1]=fProducts_res[1];
   }else  if(idpart==idUpsilon){
    fProducts_upsilon[0]=fProducts_res[0];
    fProducts_upsilon[1]=fProducts_res[1];
   }

 }
  
 return;
}
