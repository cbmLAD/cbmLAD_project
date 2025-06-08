# cbmLAD: Logical Analysis of Data for Condition-Based Maintenance

**cbmLAD** is a software tool implementing the **Logical Analysis of Data (LAD)** methodology, tailored for **Condition-Based Maintenance** applications. It leverages data pattern mining to detect failure signatures and classify machine conditions from sensor data.

## 🔧 Core Components

- **Boolean Logic**: Binarization of features and cut points generation.
- **Combinatorics**: Patterns' generation from binary data.
- **Optimization**: Uses Ant Colony Optimization (ACO) to find the optimal support set.

---

## 📁 cbmLAD Instructions

### 📂 Directory Structure

A folder named `cbmLAD` must be created on the **C drive**: `C:\\cbmLAD`

This directory will include:
-  3 Input files (.txt` files)
- Output files (automatically generated during execution)

### 📥 Input Files

Three input files must be created before running the software:

#### 1. `raw data.txt`

**Columns:**
- Column 1: Class labels 1, 2, 3,... (must not include `0`)
- Remaining columns: Feature values  
  (Nominal values must be **converted to discrete integers**)

**Rows:**
- Row 1: Feature names
- Row 2: Feature types: Nominal, binary, or numerical
- Row 3: sensors' readings or data

| Type | Description |
|------|-------------|
| 1    | Nominal (e.g., red, green → must be converted to numbers) |
| 2    | Binary (0 or 1) |
| 3    | Numerical (integer or real numbers) |

- Remaining rows: Data samples

**Important Notes:**
- First cell in row 2 (top-left) must be `3`.
- Class labels **must not include 0**.  
  Example: If original classes are `{0, 1, 2}`, change to `{1, 2, 3}`.

This is an example of the raw data file.

<img src="Images/rawdata.png" style="width:40%;"> 

#### 2. `test data.txt`

Same format as `raw_data.txt`, **without**:
- Feature names (row 1)
- Feature types (row 2)

This is an example of the test data file.

<img src="Images/testdata.png" style="width:40%;"> 

#### 3. `classification_data.txt`

Same format as `test_data.txt`, **without**:
- First column (class labels)

This is an example of a classification data file.

<img src="Images/classificationdata.png" style="width:35%;"> 

## 📤 Output Files

For each operation performed by cbmLAD (training, testing, and classification), specific output `.txt` files are generated and saved in the working directory `C:\\cbmLAD`.

### 🔧 4.1 Training Results

The training operation produces the following files:

- **Class names.txt**: Lists all class labels present in the dataset.
- **Duplicated and Ignored Observations.txt**: Identifies and excludes duplicated or invalid rows.
- **Characteristics of the generated patterns.txt**: Summarizes details about each generated pattern, such as coverage, relative prevalence, and weight. The following figure shows an example.

<img src="Images/Caracpatterns.png" style="width:40%;"> 

- **Binary data.txt**: This file includes the binary attributes that resulted from the binarization process. The following figure shows an example. 

<img src="Images/binarydata.png" style="width:40%;"> 

- **Coverage of Multi-Class Patterns.txt**: Indicates the observations in which the patterns generated for each class were found and includes their weights. Patterns with high weights cover more observations, thus have more explanatory power than patterns with low weights.

<img src="Images/coveragepatterns.png" style="width:40%;"> 

- **Patterns interpreted.txt**: Provides a readable representation of patterns using feature names and values, connected by logical AND operations.

<img src="Images/patternsinterpreted.png" style="width:35%;"> 

- **Cut points.txt**: Shows the thresholds (cut points) used to convert numeric and nominal values into binary form.

<img src="Images/cutpoints.png" style="width:35%;"> 
  
- **Results.txt**: The core output file that summarizes the input data and the final patterns for each class, including their weights and coverages. The order of class operations is determined by which class is internally treated by cbmLAD first.

<img src="Images/trainingresults.png" style="width:45%;"> 

### 🧪 4.2 Testing Results

After the testing process, cbmLAD generates:

- **Testing results.txt**: This file contains:
  - The predicted class for each test observation.
  - The discriminant values that indicate how strongly the membership of an observation is to one of the classes
  - The overall accuracy of the test predictions.
    
This is an example of the testing results file.

<img src="Images/testresults.png" style="width:40%;"> 

- **Confusion Matrix.txt**: Displays the confusion matrix summarizing prediction performance during testing. It shows actual vs. predicted class counts.

<img src="Images/confusionmatrix.png" style="width:35%;"> 

- **Confused Observations.txt**: Lists specific testing observations that were misclassified or inconsistently covered by patterns.

<img src="Images/confusedobs.png" style="width:35%;"> 

### 🧮 4.3 Classification Results

When performing classification (on new, unlabeled data), cbmLAD generates:

- **Classification results.txt**: This file provides:
  - Predicted class labels for new observations.
  - Discriminant values for each possible class.
  - The patterns used to justify each classification.

This is an example of the classification results file.

<img src="Images/classificationresults.png" style="width:40%;"> 

---

## 🧠 Pattern Generation Methods: One-vs-All (OVA) and One-vs-One (OVO)

cbmLAD supports two major strategies for handling **multiclass** problems: **One-vs-One (OVO)** and **One-vs-All (OVA)**.
For a given K-class dataset:
  - **OVO** assumes that there exists a separator between any two classes and builds **K(K−1)/2 binary classifiers**.
  - **OVA** assumes the existence of a single separator between each class $i$ and all other classes, and builds **K binary classifiers**.

For example **k=3**:

### 🟢 One-vs-All (OVA)

- Each class is compared **against all other classes combined**.
- Patterns are generated per class as:
  - $P_1^+, P_1^-, P_2^+, P_2^-, \\ldots$
    
![OVA Illustration](Images/OVA.png)

---

### 🔵 One-vs-One (OVO)

- Each class is compared **against one other class at a time**.
- Patterns are generated for each binary comparison:
  - $P_{1v2}, P_{2v1}, P_{1v3}, P_{3v1}, \\ldots$

![OVO Illustration](Images/OVO.png)

---

### 🔎 Notes

- **Binary classes:** Only the **OVA** method is used.
- **Multiclass:** Both **OVA** and **OVO** methods can be applied.

To perform training, classification, and testing using these methods, use the **executable files** provided in the `Executable Files` directory.


| **Task Type**      | **OVA (One-vs-All)**       | **OVO (One-vs-One)**       |
|--------------------|----------------------------|----------------------------|
| **Training**       | OVA_training.exe           | OVO_training.exe           |
| **Testing**        | OVA_testing.exe            | OVO_testing.exe            |
| **Classification** | OVA_Classification.exe     | OVO_Classification.exe     |
