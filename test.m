load #2frame.txt
X= X_2frame(:,1:2);
plot(X(:,1),X(:,2),'o')
axis equal
IDX = DBSCAN(X,0.5,30);
%%
load #2Dbframe.txt
a = X_2Dbframe;
%%
IDX = a;
k=max(IDX);
Colors=hsv(k);
Legends = {};
for i=1:k
    Xi=X(IDX==i,:);
    kk(i) = size(Xi,1);
    if i~=0
        Style = 'x';
        MarkerSize = 8;
        Color = Colors(i,:);
        Legends{end+1} = ['Cluster #' num2str(i)];
      if ~isempty(Xi)
        plot(Xi(:,1),Xi(:,2),Style,'MarkerSize',MarkerSize,'Color',Color);
      end
    end
   hold on;
end
hold on;
axis ([min(X(:,1)) max(X(:,1)) min(X(:,2)) max(X(:,2)) ]);
grid on;
legend(Legends);
legend('Location', 'NorthEastOutside');

