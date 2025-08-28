// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewpotSize;
	if (GEngine) {
		GEngine->GameViewport->GetViewportSize(ViewpotSize);

		const FVector2D ViewportCenter(ViewpotSize.X / 2.f, ViewpotSize.Y / 2.f);

		if (HUDPackage.CrosshairCenter) {
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, FLinearColor::White);
		}

		if (HUDPackage.CrosshairLeft) {
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, FLinearColor::White);
		}
		if (HUDPackage.CrosshairRight) {
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, FLinearColor::White);
		}
		if (HUDPackage.CrosshairTop) {
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter, FLinearColor::White);
		}
		if (HUDPackage.CrosshairBottom) {
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter, FLinearColor::White);
		}
	}

}

void AShooterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FLinearColor Color)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f),
		ViewportCenter.Y - (TextureHeight / 2.f)
	);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0, 0, 1, 1, Color);
}
