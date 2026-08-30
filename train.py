import torch
import torchvision.datasets as datasets
import torchvision.transforms as transforms
import torch.nn as nn
import torch.optim as optim
transform=transforms.ToTensor()
train_dataset=datasets.EMNIST(root="./data", split='balanced', train=True, download=True, transform=transform)
test_dataset = datasets.EMNIST(root="./data", split='balanced', train=False, download=True, transform=transform)
train_loader = torch.utils.data.DataLoader(dataset=train_dataset, batch_size=64, shuffle=True)
test_loader = torch.utils.data.DataLoader(dataset=test_dataset, batch_size=64, shuffle=False)


class TinyNN(nn.Module):
    def __init__(self):
        super(TinyNN, self).__init__()
        self.flatten = nn.Flatten()
        self.output1=nn.Linear(28*28,128) 
        self.relu=nn.ReLU()
        self.output2=nn.Linear(128,47)
    def forward(self, x):
          x = self.flatten(x)
          x = self.output1(x)
          x = self.relu(x)
          x = self.output2(x)
          return x


model=TinyNN() 
criterion=nn.CrossEntropyLoss() 
optimizer=optim.Adam(model.parameters(), lr=0.001) 

num_epochs = 5
for epoch in range(num_epochs):
    for images, labels in train_loader:
        outputs = model(images)
        loss = criterion(outputs, labels)
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
    print(f"Finished Epoch {epoch+1}!")
torch.save(model.state_dict(), "tiny_nn.pth")
print('Model saved')


correct = 0
total = 0

with torch.no_grad():
    for images, labels in test_loader:        
        outputs = model(images)                  
        _, predicted = torch.max(outputs, 1)  
        total += labels.size(0)
        correct += (predicted == labels).sum().item()  


print(f"Test Accuracy: {100 * correct / total:.2f}%")
