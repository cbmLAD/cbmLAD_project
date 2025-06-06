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

#### 2. `test data.txt`

Same format as `raw_data.txt`, **without**:
- Feature names (row 1)
- Feature types (row 2)

#### 3. `classification_data.txt`

Same format as `test_data.txt`, **without**:
- First column (class labels)

---

## 📤 Output Files

For each operation performed by cbmLAD (training, testing, and classification), specific output `.txt` files are generated and saved in the working directory `C:\\cbmLAD`.

### 🔧 4.1 Training Results

The training operation produces the following files:

- **Binary data.txt**: Contains the binarized version of the input data after applying the cut points to numeric and nominal features.
- **Characteristics of the generated patterns.txt**: Summarizes details about each generated pattern, such as coverage, relative prevalence, and weight.
- **Class names.txt**: Lists all class labels present in the dataset.
- **Coverage of Multi-Class Patterns.txt**: Indicates which observations are covered by each pattern and includes their weights. Patterns with high weights cover more observations, thus have more explanatory power than patterns with low weights.
- **Cut points.txt**: Shows the thresholds (cut points) used to convert numeric and nominal values into binary form.
- **Duplicate and ignored observations.txt**: Identifies and excludes duplicated or invalid rows.
- **Pattern matrix.txt**: Binary matrix representing which observations are covered by which patterns.
- **Patterns interpreted.txt**: Provides a readable representation of patterns using feature names and values, connected by logical AND operations.
- **Results.txt**: The core output file that summarizes the input data and the final patterns for each class, including their weights and coverages. The internal order of classes is determined by the sequence cbmLAD uses during processing.

### 🧪 4.2 Testing Results

After the testing process, cbmLAD generates:

- **Testing results.txt**: This file contains:
  - The predicted class for each test observation.
  - The discriminant values that indicate how strongly the membership of an observation is to one of the classes
  - The overall accuracy of the test predictions.  
- **Confusion Matrix.txt**: Displays the confusion matrix summarizing prediction performance during testing. It shows actual vs. predicted class counts.
- **Confused Observations.txt**: Lists specific testing observations that were misclassified or inconsistently covered by patterns. 

### 🧮 4.3 Classification Results

When performing classification (on new, unlabeled data), cbmLAD generates:

- **Classification results.txt**: This file provides:
  - Predicted class labels for new observations.
  - Discriminant values for each possible class.
  - The patterns used to justify each classification.

---

## 🧠 Pattern Generation Methods: One-vs-All (OVA) and One-vs-One (OVO)

cbmLAD supports two major strategies for handling **multiclass** problems: **One-vs-One (OvO)** and **One-vs-All (OvA)**.
For a given K-class dataset:
  - **OvO** assumes that there exists a separator between any two classes and builds **K(K−1)/2 binary classifiers**.
  - **OvA** assumes the existence of a single separator between each class $i$ and all other classes, and builds **K binary classifiers**.

For example **k=3**:

### 🟢 One-vs-All (OvA)

- Each class is compared **against all other classes combined**.
- Patterns are generated per class as:
  - $P_1^+, P_1^-, P_2^+, P_2^-, \\ldots$
    
![OVA Illustration](OVA.png)

---

### 🔵 One-vs-One (OvO)

- Each class is compared **against one other class at a time**.
- Patterns are generated for each binary comparison:
  - $P_{1v2}, P_{2v1}, P_{1v3}, P_{3v1}, \\ldots$

![OVO Illustration](OVO.png)

---

### 🔎 Notes

- **Binary classes:** Only the **OvA** method is used.
- **Multiclass:** Both **OvA** and **OvO** methods can be applied.

To perform training, classification, and testing using these methods, use the **executable files** provided in the `Executable Files` directory.
