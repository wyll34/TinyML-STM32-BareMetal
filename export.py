import torch
import numpy as np

def quantize_symmetric(weights_tensor):
    w = weights_tensor.detach().numpy()
    max_val = np.max(np.abs(w))
    scale = max_val / 127.0
    q_w = np.round(w / scale)
    q_w = np.clip(q_w, -127, 127).astype(np.int8)
    return q_w, scale

if __name__ == "__main__":
    weights_dict = torch.load("tiny_nn.pth", weights_only=True)
    
    q_layer1_w, scale1_w = quantize_symmetric(weights_dict['output1.weight'])
    q_layer1_b, scale1_b = quantize_symmetric(weights_dict['output1.bias'])
    
    q_layer2_w, scale2_w = quantize_symmetric(weights_dict['output2.weight'])
    q_layer2_b, scale2_b = quantize_symmetric(weights_dict['output2.bias'])
    
    with open("model_weights.h", "w") as f:
        f.write("#ifndef MODEL_WEIGHTS_H\n#define MODEL_WEIGHTS_H\n\n")
        f.write("#include <stdint.h>\n\n")
        
        f.write(f"const float LAYER1_SCALE = {scale1_w:.6f}f;\n")
        f.write(f"const float LAYER2_SCALE = {scale2_w:.6f}f;\n\n")
        
        def write_c_array(name, data):
            flat = data.flatten()
            f.write(f"const int8_t {name}[{len(flat)}] = {{\n    ")
            f.write(", ".join(map(str, flat)))
            f.write("\n};\n\n")

        write_c_array("layer1_weights", q_layer1_w)
        write_c_array("layer1_bias", q_layer1_b)
        write_c_array("layer2_weights", q_layer2_w)
        write_c_array("layer2_bias", q_layer2_b)
        
        f.write("#endif\n")
        
    print("Generated model_weights.h successfully.")