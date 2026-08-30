import torch
import torchvision.datasets as datasets
import torchvision.transforms as transforms
import numpy as np

weights_dict = torch.load("tiny_nn.pth", weights_only=True)

def quantize_sym(tensor):
    w = tensor.detach().numpy()
    max_val = np.max(np.abs(w))
    scale = max_val / 127.0
    return np.clip(np.round(w / scale), -127, 127).astype(np.int8)

w1 = quantize_sym(weights_dict['output1.weight'])
b1 = quantize_sym(weights_dict['output1.bias']).astype(np.int32)
w2 = quantize_sym(weights_dict['output2.weight'])
b2 = quantize_sym(weights_dict['output2.bias']).astype(np.int32)

test_dataset = datasets.EMNIST(root="./data", split='balanced', train=False, download=True, transform=transforms.ToTensor())

correct = 0
total = len(test_dataset)

for i in range(total):
    img_tensor, label = test_dataset[i]
    x = (img_tensor.squeeze().numpy() * 127.0).astype(np.int8).flatten()
    
    l1_out = np.dot(w1.astype(np.int32), x.astype(np.int32)) + b1
    l1_out[l1_out < 0] = 0
    
    l2_out = np.dot(w2.astype(np.int32), l1_out) + b2
    pred = np.argmax(l2_out)
    
    if pred == label:
        correct += 1
        
    if (i + 1) % 4000 == 0:
        print(f"Processed {i + 1}/{total} images... Accuracy: {(correct / (i + 1)) * 100:.2f}%")

final_acc = (correct / total) * 100
print(f"Test Accuracy: {final_acc:.2f}% ({correct}/{total} Correct)")