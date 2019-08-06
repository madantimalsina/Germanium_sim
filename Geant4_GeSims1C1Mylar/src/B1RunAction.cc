//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1RunAction.cc
/// \brief Implementation of the B1RunAction class

#include "B1RunAction.hh"
#include "B1PrimaryGeneratorAction.hh"
#include "B1DetectorConstruction.hh"
#include "B1Analysis.hh"
// #include "B1Run.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1RunAction::B1RunAction()
: G4UserRunAction(),
  fEdep(0.),
  fEdep1(0.),
  fEdep2(0.),
  fEdep3(0.),
  fEdep4(0.),
  fEdep5(0.)
{ 
  // add new units for dose
  // 
  const G4double milligray = 1.e-3*gray;
  const G4double microgray = 1.e-6*gray;
  const G4double nanogray  = 1.e-9*gray;  
  const G4double picogray  = 1.e-12*gray;
   
  new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray); 

  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fEdep);
  accumulableManager->RegisterAccumulable(fEdep2); 
  accumulableManager->RegisterAccumulable(fEdep4); 
  
  // Analysis Manager creating histogram
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;
  
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFileName("LArGe");
  
  // Creating histograms
  analysisManager->CreateH1("Edep","Energy deposted in C window for 1173.237 keV gammas", 20001, -0.0005*MeV, 20.0005*MeV);
  analysisManager->CreateH1("Edep1","Energy deposited in Ge detector for 1173.237 keV gammas", 20001, -0.0005*MeV, 20.0005*MeV);
  analysisManager->CreateH1("Edep4","Energy deposited in source window for 1173.237 keV gammas", 20001, -0.0005*MeV, 20.0005*MeV);
  
  analysisManager->CreateNtuple("LArGe", "Edep");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("Edep1");
  analysisManager->CreateNtupleDColumn("Edep4");
  
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1RunAction::~B1RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::BeginOfRunAction(const G4Run*)
{ 
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // Merge accumulables 
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile();

  // Compute dose = total energy deposit in a run and its variance
  //
  G4double edep  = fEdep.GetValue();
  G4double edep1 = fEdep1.GetValue();
  G4double edep2 = fEdep2.GetValue();
  G4double edep3 = fEdep3.GetValue();
  G4double edep4 = fEdep4.GetValue();
  G4double edep5 = fEdep5.GetValue();
  
  G4double rms = edep2 - edep*edep/nofEvents;
  if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;
  
  G4double rms1 = edep3 - edep1*edep1/nofEvents;
  if (rms1 > 0.) rms1 = std::sqrt(rms1); else rms1 = 0.; 
  
  G4double rms2 = edep5 - edep4*edep4/nofEvents;
  if (rms1 > 0.) rms2 = std::sqrt(rms2); else rms2 = 0.;

  const B1DetectorConstruction* detectorConstruction
   = static_cast<const B1DetectorConstruction*>
     (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
     
  G4double mass = detectorConstruction->GetScoringVolume()->GetMass();
  G4double dose = edep/mass;
  G4double rmsDose = rms/mass;
  
  G4double mass1 = detectorConstruction->GetScoringVolume1()->GetMass();
  G4double dose1 = edep1/mass1;
  G4double rmsDose1 = rms1/mass1;
  
  G4double mass2 = detectorConstruction->GetScoringVolume2()->GetMass();
  G4double dose2 = edep4/mass2;
  G4double rmsDose2 = rms2/mass2;

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const B1PrimaryGeneratorAction* generatorAction
   = static_cast<const B1PrimaryGeneratorAction*>
     (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  if (generatorAction)
  {
    /*
    const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
    runCondition += particleGun->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleGun->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy,"Energy");
    */
    
    const G4GeneralParticleSource* particleSource = generatorAction->GetParticleSource();
    runCondition += particleSource->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleSource->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy,"Energy");
    
  }
        
  // Print
  //  
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }
  
  G4cout
     << G4endl
     << " C Window: The run consists of " << nofEvents << " "<< runCondition
     << G4endl
     << " Energy deposited in C Window: " << edep
     << G4endl
     << " Cumulated dose per run, in scoring volume : " 
     << G4BestUnit(dose,"Dose") << " rms = " << G4BestUnit(rmsDose,"Dose")
     << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl
     << " Ge Detector: The run consists of " << nofEvents << " "<< runCondition
     << G4endl
     << " Energy deposited in Ge Detector: " << edep1
     << G4endl
     << " Cumulated dose per run, in scoring volume : " 
     << G4BestUnit(dose1,"Dose") << " rms = " << G4BestUnit(rmsDose1,"Dose")
     << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl
     << " Source Window: The run consists of " << nofEvents << " "<< runCondition
     << G4endl
     << " Energy deposited in Source Window: " << edep1
     << G4endl
     << " Cumulated dose per run, in scoring volume : " 
     << G4BestUnit(dose1,"Dose") << " rms = " << G4BestUnit(rmsDose1,"Dose")
     << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;
     
     // save histograms & ntuple
     //
     analysisManager->Write();
     analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1RunAction::AddEdep(G4double edep)
{
  fEdep  += edep;
  fEdep2 += edep*edep;
}

void B1RunAction::AddEdep1(G4double edep1)
{
  fEdep1  += edep1;
  fEdep3 += edep1*edep1;
}

void B1RunAction::AddEdep4(G4double edep4)
{
  fEdep4  += edep4;
  fEdep5 += edep4*edep4;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

