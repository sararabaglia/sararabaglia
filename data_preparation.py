import ROOT
 
 
def filter_events(df):
    """
    Reduce initial dataset to only events which shall be used for training
    """
    return df.Filter("nElectron>=2 && nMuon>=2", "At least two electrons and two muons")
 
 
def define_variables(df):
    """
    Define the variables which shall be used for training
    """
    return df.Define("Length")\
             .Define("Width")\
             .Define("Size")\
             .Define("Conc")\
             .Define("Conc1")\
             .Define("Asym")\
             .Define("M3Long")\
             .Define("M3Trans")\
             .Define("Alpha")\
             .Define("Dist")
 
 
variables = ["Length", "Width", "Size", "Conc", "Conc1", "Asym", "M3Long", "M3Trans", "Alpha", "Dist"]
 
 
if __name__ == "__main__":
    for filename, label in [["magic.root", "signalTree"], ["magic.root", "bkgTree"]]:
        print(">>> Extract the training and testing events for {} from the {} dataset.".format(
            label, filename))
 
        # Load dataset, filter the required events and define the training variables
        filepath = "/home/sara/tmva_prova" + filename
        df = ROOT.RDataFrame("Events", filepath)
        df = filter_events(df)
        df = define_variables(df)
 
        # Book cutflow report
        report = df.Report()
 
        # Split dataset by event number for training and testing
        columns = ROOT.std.vector["string"](variables)
        df.Filter("event % 2 == 0", "Select events with even event number for training")\
          .Snapshot("Events", "train_" + label + ".root", columns)
        df.Filter("event % 2 == 1", "Select events with odd event number for training")\
          .Snapshot("Events", "test_" + label + ".root", columns)
 
        # Print cutflow report
        report.Print()
