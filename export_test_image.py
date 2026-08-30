import torchvision.datasets as datasets
import torchvision.transforms as transforms
import numpy as np

test_dataset = datasets.EMNIST(root="./data", split='balanced', train=False, download=True, transform=transforms.ToTensor())
img, label = test_dataset[0]
img_int8 = (img.squeeze().numpy() * 127.0).astype(np.int8).flatten()

with open("test_image.h", "w") as f:
    f.write("#ifndef TEST_IMAGE_H\n#define TEST_IMAGE_H\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write(f"const int TEST_IMAGE_LABEL = {label};\n\n")
    f.write(f"const int8_t test_image_data[784] = {{\n    ")
    f.write(", ".join(map(str, img_int8)))
    f.write("\n};\n\n")
    f.write("#endif\n")

print("Generated test_image.h")