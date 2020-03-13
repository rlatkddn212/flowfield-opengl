# flowfield-opengl
c++로 flow field 알고리즘 개발

1. flow field
2. 화살표 표시
3. Object 추가
4. Object 이동(주변 4개 타일을 이중선형 보간, bilinear interpolation)

마우스 왼쪽 클릭 - 지형생성  
마우스 오른쪽 클릭 - 이동(flow field 생성)

![test12341](https://github.com/rlatkddn212/flowfield-opengl/blob/master/flowfield.gif)

#### 구현 내용
- 다익스트라를 사용하여 flow field를 생성한 후 방향을 따라 오브젝트가 이동하도록 구현

#### bug?  
- 방향을 이중 선형 보간으로 처리하는데 선택한 칸들이 서로 다른 방향을 가르킬 경우 방향이 상쇄되 이동 하지 않는다.
- 비슷한 이유로 방향이 보간 처리되기 때문에 장애물을 통과하는 경우가 있다.

#### 추가해볼 사항
충돌 처리 혹은 충돌 회피  
RVO2도 사용해봐야겠다.

#### 성능
10만개 오브젝트를 추가해도 문제없이 동작한다.

#### 참고 자료
https://gamedevelopment.tutsplus.com/tutorials/understanding-goal-based-vector-field-pathfinding--gamedev-9007  
https://howtorts.github.io/2014/01/04/basic-flow-fields.html  
http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter23_Crowd_Pathfinding_and_Steering_Using_Flow_Field_Tiles.pdf
