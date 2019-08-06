// ROOT macro file for plotting example He3 histograms 
// 
// Can be run from ROOT session:
// root[0] .x plotHisto.C

{
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  
  // Draw histos filled by Geant4 simulation 
  //   

  // Open file filled by Geant4 simulation 
  TFile f("LArGe.root");

  TH1D* hist1 = (TH1D*)f.Get("Edep");
  c1->SetOption("E");
  c1->GetXAxis()->SetTitle("stuff");
  //TH1* h1 = new TH1D("h1", "h1 title", 201, -0.5, 20.0);
  c1->Draw("HIST");
}  
